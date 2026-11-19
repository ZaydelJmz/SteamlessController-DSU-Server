#include "ViGEmBusInfo.h"

#include <windows.h>
#include <setupapi.h>
#include <cfgmgr32.h>

// GUID_DEVINTERFACE_BUSENUM_VIGEM. Taken from the vendored client rather than
// copied, so updating third_party cannot leave a stale value behind here — the
// header itself tells forks to change the GUID, which is the whole reason this
// file exists. Declared here and defined in ViGEmClient.obj, which includes
// <initguid.h> ahead of the same header.
#include <ViGEm/km/BusShared.h>

namespace {

// SPDRP_FRIENDLYNAME is what Device Manager shows and what a fork rebrands:
// "Nefarius Virtual Gamepad Emulation Bus" against "Oculus Virtual Gamepad
// Emulation Bus". Not every devnode sets one, so fall back to its description.
std::wstring ReadName(HDEVINFO set, SP_DEVINFO_DATA& info) {
    for (const DWORD prop : {SPDRP_FRIENDLYNAME, SPDRP_DEVICEDESC}) {
        DWORD bytes = 0;
        SetupDiGetDeviceRegistryPropertyW(set, &info, prop, nullptr, nullptr, 0, &bytes);
        if (bytes == 0) continue;

        std::wstring value(bytes / sizeof(wchar_t) + 1, L'\0');
        if (!SetupDiGetDeviceRegistryPropertyW(set, &info, prop, nullptr,
                                               reinterpret_cast<PBYTE>(value.data()),
                                               bytes, nullptr))
            continue;

        value.resize(wcslen(value.c_str()));
        if (!value.empty()) return value;
    }
    return {};
}

}  // namespace

std::vector<ViGEmBusInfo::Bus> ViGEmBusInfo::Enumerate() {
    std::vector<Bus> buses;

    const HDEVINFO set = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_BUSENUM_VIGEM,
                                              nullptr, nullptr,
                                              DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (set == INVALID_HANDLE_VALUE) return buses;

    SP_DEVICE_INTERFACE_DATA iface{};
    iface.cbSize = sizeof(iface);
    for (DWORD i = 0;
         SetupDiEnumDeviceInterfaces(set, nullptr, &GUID_DEVINTERFACE_BUSENUM_VIGEM,
                                     i, &iface);
         ++i) {
        DWORD bytes = 0;
        SetupDiGetDeviceInterfaceDetailW(set, &iface, nullptr, 0, &bytes, nullptr);
        if (bytes == 0) continue;

        std::vector<BYTE> buffer(bytes);
        auto* detail = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA_W>(buffer.data());
        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

        SP_DEVINFO_DATA info{};
        info.cbSize = sizeof(info);
        if (!SetupDiGetDeviceInterfaceDetailW(set, &iface, detail, bytes, nullptr, &info))
            continue;

        Bus bus;
        bus.devicePath   = detail->DevicePath;
        bus.friendlyName = ReadName(set, info);

        wchar_t devId[MAX_DEVICE_ID_LEN] = {0};
        if (CM_Get_Device_IDW(info.DevInst, devId, MAX_DEVICE_ID_LEN, 0) == CR_SUCCESS) {
            bus.instanceId = devId;
        }

        DEVINST childDev = 0;
        if (CM_Get_Child(&childDev, info.DevInst, 0) == CR_SUCCESS) {
            while (childDev) {
                ULONG status = 0, problem = 0;
                if (CM_Get_DevNode_Status(&status, &problem, childDev, 0) == CR_SUCCESS) {
#ifndef DN_WILL_BE_REMOVED
                    constexpr ULONG DN_WILL_BE_REMOVED = 0x00040000;
#endif
                    if (!(status & DN_WILL_BE_REMOVED)) {
                        wchar_t childId[MAX_DEVICE_ID_LEN] = {0};
                        if (CM_Get_Device_IDW(childDev, childId, MAX_DEVICE_ID_LEN, 0) == CR_SUCCESS) {
                            bus.children.push_back(childId);
                        }
                    }
                }
                DEVINST nextSibling = 0;
                if (CM_Get_Sibling(&nextSibling, childDev, 0) != CR_SUCCESS) break;
                childDev = nextSibling;
            }
        }

        buses.push_back(std::move(bus));
    }

    SetupDiDestroyDeviceInfoList(set);
    return buses;
}

bool ViGEmBusInfo::HasAnyChildren() {
    for (const auto& bus : Enumerate()) {
        if (!bus.children.empty()) return true;
    }
    return false;
}

std::vector<std::wstring> ViGEmBusInfo::GetAllChildren() {
    std::vector<std::wstring> all;
    for (const auto& bus : Enumerate()) {
        for (const auto& child : bus.children) {
            all.push_back(child);
        }
    }
    return all;
}

std::wstring ViGEmBusInfo::Describe(const std::vector<Bus>& buses) {
    if (buses.empty())
        return L"no virtual gamepad bus is present — ViGEmBus is not installed, "
               L"or its device is disabled in Device Manager under System devices";

    // The count leads, because more than one is the answer to the question a
    // maintainer reading this line is actually asking.
    std::wstring text = std::to_wstring(buses.size());
    text += (buses.size() == 1)
          ? L" virtual gamepad bus present: "
          : L" virtual gamepad buses present — the client connects to whichever "
            L"answers first, which may not be the one this app needs: ";

    for (size_t i = 0; i < buses.size(); ++i) {
        if (i != 0) text += L", ";
        text += buses[i].friendlyName.empty() ? buses[i].devicePath
                                              : buses[i].friendlyName;
    }
    return text;
}

void ViGEmBusInfo::UnplugAllTargets() {
    const auto buses = Enumerate();
    for (const auto& bus : buses) {
        HANDLE hBus = CreateFileW(
            bus.devicePath.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH | FILE_FLAG_OVERLAPPED,
            nullptr
        );
        if (hBus == INVALID_HANDLE_VALUE) continue;

        OVERLAPPED ol{};
        ol.hEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

        for (ULONG serial = 1; serial <= 16; ++serial) {
            VIGEM_UNPLUG_TARGET unplug{};
            unplug.Size = sizeof(unplug);
            unplug.SerialNo = serial;
            DWORD transferred = 0;
            BOOL ok = DeviceIoControl(
                hBus,
                IOCTL_VIGEM_UNPLUG_TARGET,
                &unplug,
                sizeof(unplug),
                nullptr,
                0,
                &transferred,
                &ol
            );
            DWORD err = GetLastError();
            if (ol.hEvent) {
                BOOL res = GetOverlappedResult(hBus, &ol, &transferred, TRUE);
                wprintf(L"  Serial %u: DeviceIoControl=%d, GetOverlappedResult=%d (err=%lu)\n", serial, ok, res, GetLastError());
            } else {
                wprintf(L"  Serial %u: DeviceIoControl=%d (err=%lu)\n", serial, ok, err);
            }
        }

        if (ol.hEvent) CloseHandle(ol.hEvent);
        CloseHandle(hBus);
    }
}

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#include "ViiperClient.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cctype>
#include <cstdio>
#include <string>
#include <vector>

namespace {

constexpr ULONGLONG kFailBackoffMs = 25000;   // under TrayApp's 30 s VIGEM_RETRY_MS
std::atomic<ULONGLONG> g_lastFailTick{0};

// The sidecar is started once and kept for the life of the app. Starting it per
// attempt cost up to seconds on the UI thread each time and churned processes.
struct SidecarProcess {
    std::mutex mutex;
    HANDLE     handle = nullptr;
    ~SidecarProcess() {
        if (!handle) return;
        TerminateProcess(handle, 0);
        WaitForSingleObject(handle, 2000);
        CloseHandle(handle);
    }
    bool Running() {
        if (handle && WaitForSingleObject(handle, 0) == WAIT_OBJECT_0) {
            CloseHandle(handle);
            handle = nullptr;
        }
        return handle != nullptr;
    }
} g_sidecar;

constexpr std::uintptr_t kInvalidSocket = ~static_cast<std::uintptr_t>(0);
constexpr uint16_t kViiperPort = 3242;
constexpr size_t   kFeedbackSize = 10;  // 4 left + 4 right HD Rumble, flags, player LEDs

bool EnsureWinsock() {
    static const bool ok = [] { WSADATA d{}; return WSAStartup(MAKEWORD(2, 2), &d) == 0; }();
    return ok;
}

void CloseSocket(std::uintptr_t& v) {
    if (v == kInvalidSocket) return;
    SOCKET s = static_cast<SOCKET>(v);
    shutdown(s, SD_BOTH);
    closesocket(s);
    v = kInvalidSocket;
}

bool SendAll(SOCKET s, const char* data, size_t size) {
    for (size_t sent = 0; sent < size;) {
        const int n = send(s, data + sent, static_cast<int>(std::min<size_t>(size - sent, 16384)), 0);
        if (n <= 0) return false;
        sent += static_cast<size_t>(n);
    }
    return true;
}

bool ReceiveExact(SOCKET s, uint8_t* data, size_t size) {
    for (size_t got = 0; got < size;) {
        const int n = recv(s, reinterpret_cast<char*>(data + got), static_cast<int>(size - got), 0);
        if (n <= 0) return false;
        got += static_cast<size_t>(n);
    }
    return true;
}

bool ReceiveUntilClose(SOCKET s, std::string& out) {
    out.clear();
    std::array<char, 4096> buf{};
    for (;;) {
        const int n = recv(s, buf.data(), static_cast<int>(buf.size()), 0);
        if (n > 0) { out.append(buf.data(), static_cast<size_t>(n)); continue; }
        if (n == 0) break;
        return false;
    }
    if (!out.empty() && out.back() == '\n') out.pop_back();
    return !out.empty();
}

SOCKET ConnectLoopback(int timeoutMs) {
    if (!EnsureWinsock()) return INVALID_SOCKET;
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) return INVALID_SOCKET;

    const int noDelay = 1;
    setsockopt(s, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&noDelay), sizeof(noDelay));
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeoutMs), sizeof(timeoutMs));
    setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&timeoutMs), sizeof(timeoutMs));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(kViiperPort);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    if (connect(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        closesocket(s);
        return INVALID_SOCKET;
    }
    return s;
}

std::wstring ExeDirectory() {
    std::vector<wchar_t> buf(32768);
    const DWORD len = GetModuleFileNameW(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
    if (len == 0 || len >= buf.size()) return {};
    std::wstring path(buf.data(), len);
    const size_t slash = path.find_last_of(L"\\/");
    return slash == std::wstring::npos ? std::wstring{} : path.substr(0, slash);
}

bool FileExists(const std::wstring& path) {
    const DWORD a = GetFileAttributesW(path.c_str());
    return a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY);
}

std::wstring ViiperLogPath() {
    wchar_t base[MAX_PATH];
    const DWORD n = GetEnvironmentVariableW(L"LOCALAPPDATA", base, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) return {};
    const std::wstring dir = std::wstring(base) + L"\\SteamlessController";
    CreateDirectoryW(dir.c_str(), nullptr);
    return dir + L"\\viiper.log";
}

// The narrow JSON this server answers with is flat; a key lookup is enough.
size_t JsonValueStart(const std::string& json, const char* key) {
    const std::string token = std::string("\"") + key + "\"";
    size_t pos = json.find(token);
    if (pos == std::string::npos) return pos;
    pos = json.find(':', pos + token.size());
    if (pos == std::string::npos) return pos;
    ++pos;
    while (pos < json.size() && std::isspace(static_cast<unsigned char>(json[pos]))) ++pos;
    return pos;
}

bool JsonString(const std::string& json, const char* key, std::string& value) {
    size_t pos = JsonValueStart(json, key);
    if (pos >= json.size() || json[pos] != '"') return false;
    std::string out;
    for (++pos; pos < json.size(); ++pos) {
        if (json[pos] == '\\' && pos + 1 < json.size()) { out.push_back(json[++pos]); continue; }
        if (json[pos] == '"') { value = out; return true; }
        out.push_back(json[pos]);
    }
    return false;
}

bool JsonUint(const std::string& json, const char* key, uint32_t& value) {
    size_t pos = JsonValueStart(json, key);
    uint64_t v = 0;
    bool digit = false;
    for (; pos < json.size() && json[pos] >= '0' && json[pos] <= '9'; ++pos) {
        digit = true;
        v = v * 10 + static_cast<uint64_t>(json[pos] - '0');
        if (v > 0xFFFFFFFFull) return false;
    }
    if (digit) value = static_cast<uint32_t>(v);
    return digit;
}

std::string Lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

} // namespace

// Stock VIIPER has no nspro device; only the SteamlessController-patched
// v0.6.1 build tagged "steamless8" or later carries it (see tools/build-viiper.ps1).
bool IsViiperSwitchProCompatibleVersion(const std::string& version) {
    const std::string v = Lower(version);
    const size_t base = v.rfind("v0.6.1", 0) == 0 ? 6 : v.rfind("0.6.1", 0) == 0 ? 5 : 0;
    if (base == 0 || (base < v.size() && std::isdigit(static_cast<unsigned char>(v[base]))))
        return false;   // not 0.6.1, or 0.6.1x
    const size_t at = v.find("steamless", base);
    if (at == std::string::npos) return false;
    int n = 0;
    bool digit = false;
    for (size_t i = at + 9; i < v.size() && v[i] >= '0' && v[i] <= '9'; ++i) {
        n = n * 10 + (v[i] - '0');
        digit = true;
    }
    return digit && n >= 8;
}

ViiperClient::~ViiperClient() { Close(); }

bool ViiperClient::Fail(const char* stage, const wchar_t* message) {
    m_failStage    = stage;
    m_errorMessage = message;
    g_lastFailTick = GetTickCount64();
    return false;
}

bool ViiperClient::RecentlyFailed() {
    const ULONGLONG at = g_lastFailTick.load();
    return at != 0 && GetTickCount64() - at < kFailBackoffMs;
}

bool ViiperClient::Open(const char* deviceType, FeedbackFn feedbackFn) {
    Close();
    m_failStage = "none";
    m_errorMessage.clear();
    m_deviceType = (deviceType && *deviceType) ? deviceType : "nspro";
    m_feedbackFn = std::move(feedbackFn);

    if (!EnsureServerReady() || !CreateBus()) return false;
    if (!AddDevice() || !OpenStream()) {
        RemoveDeviceAndBus();
        return false;
    }
    m_open = true;
    g_lastFailTick = 0;
    return true;
}

void ViiperClient::Close() {
    m_feedbackRunning = false;
    CloseSocket(m_streamSocket);
    if (m_feedbackThread.joinable()) m_feedbackThread.join();
    RemoveDeviceAndBus();
    m_open = false;
    m_feedbackFn = {};
}

bool ViiperClient::SendInput(const uint8_t* data, size_t size) {
    if (!m_open || m_streamSocket == kInvalidSocket) return false;
    std::lock_guard<std::mutex> lock(m_sendMutex);
    return SendAll(static_cast<SOCKET>(m_streamSocket), reinterpret_cast<const char*>(data), size);
}

bool ViiperClient::EnsureServerReady() {
    if (PingServer()) return true;
    if (m_unsupported) return false;
    std::lock_guard<std::mutex> lock(g_sidecar.mutex);
    if (!g_sidecar.Running() && !SpawnBundledServer()) return false;
    for (int i = 0; i < 40; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (PingServer()) return true;
        if (m_unsupported) return false;
    }
    return Fail("viiper_start", L"viiper.exe started but did not answer on 127.0.0.1:3242.");
}

bool ViiperClient::PingServer() {
    m_unsupported = false;
    std::string response, server, version;
    if (!Request("ping", {}, response)
            || !JsonString(response, "server", server) || !JsonString(response, "version", version))
        return Fail("viiper_ping", L"VIIPER server is not reachable.");
    if (server != "VIIPER" || !IsViiperSwitchProCompatibleVersion(version)) {
        m_unsupported = true;
        return Fail("viiper_version",
                    L"The running VIIPER server has no Switch Pro support. Use the patched "
                    L"viiper.exe (v0.6.1-steamless8 or later) and close any other VIIPER.");
    }
    return true;
}

bool ViiperClient::SpawnBundledServer() {
    const std::wstring dir  = ExeDirectory();
    const std::wstring path = dir + L"\\viiper.exe";
    if (dir.empty() || !FileExists(path))
        return Fail("viiper_missing", L"viiper.exe was not found next to SteamlessController.exe.");

    std::wstring command = L"\"" + path + L"\" server --update-notify none";
    const std::wstring log = ViiperLogPath();
    if (!log.empty()) command += L" \"--log.file=" + log + L"\"";
    std::vector<wchar_t> cmdLine(command.begin(), command.end());
    cmdLine.push_back(L'\0');

    STARTUPINFOW si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi{};
    if (!CreateProcessW(path.c_str(), cmdLine.data(), nullptr, nullptr, FALSE,
                        CREATE_NO_WINDOW, nullptr, dir.c_str(), &si, &pi))
        return Fail("viiper_start", L"Failed to start viiper.exe.");
    CloseHandle(pi.hThread);
    g_sidecar.handle = pi.hProcess;
    return true;
}

bool ViiperClient::Request(const std::string& path, const std::string& payload, std::string& response) {
    SOCKET s = ConnectLoopback(2000);   // loopback: a live server answers in ms
    if (s == INVALID_SOCKET) return false;
    std::string req = payload.empty() ? path : path + " " + payload;
    req.push_back('\0');
    const bool ok = SendAll(s, req.data(), req.size()) && ReceiveUntilClose(s, response);
    closesocket(s);
    return ok;
}

bool ViiperClient::CreateBus() {
    std::string response;
    uint32_t busId = 0;
    if (!Request("bus/create", "0", response) || !JsonUint(response, "busId", busId) || busId == 0)
        return Fail("viiper_bus", L"Failed to create the VIIPER virtual USB bus.");
    m_busId = busId;
    return true;
}

bool ViiperClient::AddDevice() {
    std::string response;
    const std::string payload = std::string("{\"type\":\"") + m_deviceType + "\"}";
    if (!Request("bus/" + std::to_string(m_busId) + "/add", payload, response))
        return Fail("viiper_add", L"Failed to create the virtual Switch Pro Controller.");

    const std::string text = Lower(response);
    // The driver is there but speaks an interface this sidecar does not know.
    if (text.find("deviceiocontrol failed") != std::string::npos)
        return Fail("usbip_version",
                    L"This usbip-win2 version does not work with viiper.exe. Run "
                    L"\"Install Prerequisites\" to install usbip-win2 0.9.7.7, then reboot.");
    if (text.find("usbip-win2 driver not found") != std::string::npos
            || text.find("requires the usbip-win2 driver") != std::string::npos
            || text.find("failed to auto-attach device") != std::string::npos)
        return Fail("usbip_missing",
                    L"Switch Pro mode needs the usbip-win2 driver. Install it and reboot.");

    uint32_t busId = 0;
    std::string devId;
    if (!JsonUint(response, "busId", busId) || busId != m_busId
            || !JsonString(response, "devId", devId) || devId.empty())
        return Fail("viiper_add", L"Failed to create the virtual Switch Pro Controller.");
    m_devId = devId;
    return true;
}

bool ViiperClient::OpenStream() {
    SOCKET s = ConnectLoopback(0);
    std::string path = "bus/" + std::to_string(m_busId) + "/" + m_devId;
    path.push_back('\0');
    if (s == INVALID_SOCKET || !SendAll(s, path.data(), path.size())) {
        if (s != INVALID_SOCKET) closesocket(s);
        return Fail("viiper_stream", L"Failed to connect the VIIPER device stream.");
    }
    m_streamSocket = static_cast<std::uintptr_t>(s);
    m_feedbackRunning = true;
    m_feedbackThread = std::thread(&ViiperClient::FeedbackLoop, this, m_streamSocket);
    return true;
}

void ViiperClient::RemoveDeviceAndBus() {
    std::string response;
    if (m_busId != 0 && !m_devId.empty())
        Request("bus/" + std::to_string(m_busId) + "/remove", m_devId, response);
    m_devId.clear();
    if (m_busId != 0)
        Request("bus/remove", std::to_string(m_busId), response);
    m_busId = 0;
}

void ViiperClient::FeedbackLoop(std::uintptr_t streamSocket) {
    std::array<uint8_t, kFeedbackSize> buf{};
    while (m_feedbackRunning
            && ReceiveExact(static_cast<SOCKET>(streamSocket), buf.data(), buf.size())) {
        if (m_feedbackFn) m_feedbackFn(buf.data(), buf.data() + 4, buf[8]);
    }
}

void ViiperClient::StopSidecar() {
    std::lock_guard<std::mutex> lk(g_sidecar.mutex);
    if (g_sidecar.handle) {
        TerminateProcess(g_sidecar.handle, 0);
        WaitForSingleObject(g_sidecar.handle, 1000);
        CloseHandle(g_sidecar.handle);
        g_sidecar.handle = nullptr;
    }
}

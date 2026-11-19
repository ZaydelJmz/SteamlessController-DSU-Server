#pragma once
#include <string>
#include <vector>

// What virtual gamepad buses are actually present.
//
// The ViGEm client finds its bus by enumerating one interface GUID and taking
// the first device that answers, with no notion of which product published it.
// Its error codes are no better: VIGEM_ERROR_BUS_NOT_FOUND covers both "no
// driver installed" and "a bus is there and did not answer". The app used to
// infer the first from an error that means either, which sent a reporter
// hunting for a driver they did not have and could not tell they lacked (#65).
//
// So ask the system instead of guessing from an error code.
//
// Worth knowing what this does *not* see. Rebranded ViGEm forks ship with
// several products (Oculus and Virtual Desktop both ship one), and the
// upstream header instructs forks to change this GUID — measured in #68, on a
// machine carrying both of those forks, neither published it and only the
// Nefarius bus appeared. So an empty result really does mean no usable bus,
// which is what makes it safe to drive "the driver is missing" off of.
namespace ViGEmBusInfo {

// Only buses that are actually usable appear here. Measured on a disabled
// ViGEmBus: the devnode stays present and reports a problem, but its interface
// is withdrawn entirely, so a disabled bus reads the same as an absent one.
// That is why "disabled in Device Manager" belongs in the empty-list wording
// rather than in a separate state nothing ever reaches.
struct Bus {
    std::wstring devicePath;    // \\?\ROOT#SYSTEM#0001#{96e42b22-...}
    std::wstring friendlyName;  // as Device Manager shows it; may be empty
    std::wstring instanceId;    // e.g. ROOT\SYSTEM\0004
    std::vector<std::wstring> children; // e.g. USB\VID_045E&PID_028E\01
};

// Every present device publishing the ViGEm bus interface. Empty is the one
// case that genuinely means "go install the driver".
std::vector<Bus> Enumerate();

// Forcefully unplugs all virtual controller targets across all detected ViGEm buses.
// Used at startup and shutdown to clean up any orphaned devices left behind by crashes
// or improper disconnects.
void UnplugAllTargets();

// Returns true if any ViGEm bus currently has active child devices (virtual gamepads)
bool HasAnyChildren();

// Queries children of all present ViGEm buses
std::vector<std::wstring> GetAllChildren();

// One log line: how many buses answered, and what they call themselves.
std::wstring Describe(const std::vector<Bus>& buses);

}  // namespace ViGEmBusInfo

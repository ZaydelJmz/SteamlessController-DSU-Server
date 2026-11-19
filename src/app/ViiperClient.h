#pragma once

#include <cstddef>
#include <cstdint>
#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <thread>

class ViiperClient {
public:
    using FeedbackFn = std::function<void(const uint8_t* leftHaptic, const uint8_t* rightHaptic, uint8_t leds)>;

    ViiperClient() = default;
    ~ViiperClient();

    ViiperClient(const ViiperClient&) = delete;
    ViiperClient& operator=(const ViiperClient&) = delete;

    bool Open(const char* deviceType = "nspro", FeedbackFn feedbackFn = {});
    void Close();
    bool SendInput(const uint8_t* data, size_t size);

    static bool RecentlyFailed();
    bool Unsupported() const { return m_unsupported.load(); }
    const wchar_t* LastError() const { return m_errorMessage.c_str(); }

    static void StopSidecar();
    static void CleanupAllBuses();

private:
    bool EnsureServerReady();
    bool PingServer();
    bool SpawnBundledServer();
    bool Request(const std::string& path, const std::string& payload, std::string& response);
    bool CreateBus();
    bool AddDevice();
    bool OpenStream();
    void RemoveDeviceAndBus();
    void FeedbackLoop(std::uintptr_t streamSocket);
    bool Fail(const char* stage, const wchar_t* message);

    std::uintptr_t m_streamSocket = ~static_cast<std::uintptr_t>(0);
    uint32_t m_busId = 0;
    std::string m_devId;
    FeedbackFn m_feedbackFn;
    std::atomic<bool> m_open{false};
    std::atomic<bool> m_feedbackRunning{false};
    std::atomic<bool> m_unsupported{false};
    std::mutex m_sendMutex;
    std::thread m_feedbackThread;
    std::string m_failStage = "none";
    std::wstring m_errorMessage;
    std::string m_deviceType = "nspro";
};

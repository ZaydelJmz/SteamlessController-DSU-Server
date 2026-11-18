#pragma once
#include <cstdint>
#include <atomic>
#include <memory>

class DsuServer {
public:
    static constexpr uint16_t DEFAULT_PORT = 26760;

    explicit DsuServer(uint16_t port = DEFAULT_PORT);
    ~DsuServer();

    DsuServer(const DsuServer&) = delete;
    DsuServer& operator=(const DsuServer&) = delete;

    bool Start();
    void Stop();
    bool IsRunning() const;

    void SetActive(bool active);
    bool IsActive() const;

    bool HasSubscribers() const;

    void SetSlotConnected(int slot, bool connected);
    void PushReport(int slot, const uint8_t* report, size_t len);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

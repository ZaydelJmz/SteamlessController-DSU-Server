#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include "DsuServer.h"
#include "EventLog.h"
#include "steam/SteamController.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <chrono>

namespace {

const uint8_t  MAGIC_CLIENT[4] = {'D', 'S', 'U', 'C'};
const uint8_t  MAGIC_SERVER[4] = {'D', 'S', 'U', 'S'};
const uint16_t PROTO_VER       = 1001;
const uint32_t MSG_VERSION     = 0x100000;
const uint32_t MSG_PORTS       = 0x100001;
const uint32_t MSG_DATA        = 0x100002;

const size_t HEADER_LEN        = 16;
const size_t HEADER_LEN_FULL   = 20;
const size_t CTRL_HDR_LEN      = 11;
const size_t CRC_OFFSET        = 8;
const size_t CRC_LEN           = 4;
const size_t MAX_SLOTS         = 4;
const size_t MAX_SUBS          = 32;
const auto   SUB_TIMEOUT       = std::chrono::seconds(5);

const uint8_t SLOT_MACS[4][6] = {
    {0x00, 0x1B, 0x7A, 0x00, 0x00, 0x01},
    {0x00, 0x1B, 0x7A, 0x00, 0x00, 0x02},
    {0x00, 0x1B, 0x7A, 0x00, 0x00, 0x03},
    {0x00, 0x1B, 0x7A, 0x00, 0x00, 0x04}
};

uint32_t s_crc32tab[256];
bool     s_crc32ready = false;

void BuildCrc32() {
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t c = i;
        for (int j = 0; j < 8; j++) {
            c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
        }
        s_crc32tab[i] = c;
    }
    s_crc32ready = true;
}

uint32_t ComputeCrc32(const uint8_t* data, size_t len) {
    if (!s_crc32ready) BuildCrc32();
    uint32_t c = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; i++) {
        c = s_crc32tab[(c ^ data[i]) & 0xFF] ^ (c >> 8);
    }
    return c ^ 0xFFFFFFFFu;
}

uint32_t CrcZeroed(const uint8_t* msg, size_t len) {
    std::vector<uint8_t> tmp(msg, msg + len);
    if (len >= CRC_OFFSET + CRC_LEN) {
        memset(tmp.data() + CRC_OFFSET, 0, CRC_LEN);
    }
    return ComputeCrc32(tmp.data(), len);
}

inline void W16(uint8_t* p, uint16_t v) { p[0] = v & 0xFF; p[1] = (v >> 8) & 0xFF; }
inline void W32(uint8_t* p, uint32_t v) {
    p[0] = v & 0xFF; p[1] = (v >> 8) & 0xFF;
    p[2] = (v >> 16) & 0xFF; p[3] = (v >> 24) & 0xFF;
}
inline uint16_t R16(const uint8_t* p) { return p[0] | (static_cast<uint16_t>(p[1]) << 8); }
inline uint32_t R32(const uint8_t* p) {
    return p[0] | (static_cast<uint32_t>(p[1]) << 8) |
           (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
}

void WriteHeader(uint8_t* out, size_t totalLen, uint32_t sid, uint32_t mtype) {
    memcpy(out, MAGIC_SERVER, 4);
    W16(out + 4, PROTO_VER);
    W16(out + 6, static_cast<uint16_t>(totalLen - HEADER_LEN));
    memset(out + CRC_OFFSET, 0, CRC_LEN);
    W32(out + 12, sid);
    W32(out + 16, mtype);
}

void FinalizeCrc(uint8_t* out, size_t len) {
    W32(out + CRC_OFFSET, CrcZeroed(out, len));
}

void WriteCtrlHdr(uint8_t* buf, uint8_t slot, bool conn) {
    buf[0] = slot;
    if (conn && slot < MAX_SLOTS) {
        buf[1] = 2; // state = 2 (connected)
        buf[2] = 2; // model = 2 (full gyro)
        buf[3] = 1; // connection = 1 (USB)
        memcpy(buf + 4, SLOT_MACS[slot], 6);
        buf[10] = 0; // battery
    } else {
        memset(buf + 1, 0, 10);
    }
}

inline uint8_t StickToU8(int16_t v) {
    return static_cast<uint8_t>((static_cast<int32_t>(v) + 32768) >> 8);
}

inline uint8_t TriggerToU8(uint16_t v) {
    uint32_t r = v >> 7;
    return r > 255 ? 255 : static_cast<uint8_t>(r);
}

uint32_t GenerateServerId() {
    auto t = std::chrono::steady_clock::now().time_since_epoch().count();
    return static_cast<uint32_t>(t ^ (t >> 32) ^ 2654435761u);
}

} // namespace

struct DsuServer::Impl {
    SOCKET            sock = INVALID_SOCKET;
    uint16_t          port = DEFAULT_PORT;
    uint32_t          serverId = 0;
    std::atomic<bool> running{false};
    std::atomic<bool> active{true};
    std::thread       thread;

    struct Subscriber {
        struct sockaddr_in addr{};
        std::chrono::steady_clock::time_point lastRequest{};
        uint32_t packetCounters[4] = {};
    };

    mutable std::mutex subMutex;
    std::unordered_map<uint32_t, Subscriber> subscribers;

    std::mutex stateMutex;
    bool slotConnected[4] = {};

    void ServerLoop();
    void HandleMessage(const uint8_t* buf, int len, const struct sockaddr_in& src);
    void SendVersion(const struct sockaddr_in& src);
    void SendSlotInfo(const struct sockaddr_in& src, uint8_t slot);
    void BroadcastData(int slot, uint64_t timestampUs,
                       float ax, float ay, float az,
                       float gx, float gy, float gz,
                       const uint8_t* report, size_t len);
    void CleanupSubscribers();
};

void DsuServer::Impl::ServerLoop() {
    uint8_t buf[1024];
    while (running.load()) {
        struct sockaddr_in src{};
        int sl = sizeof(src);
        int n = recvfrom(sock, reinterpret_cast<char*>(buf), sizeof(buf), 0,
                         reinterpret_cast<struct sockaddr*>(&src), &sl);
        if (n > 0) {
            HandleMessage(buf, n, src);
        }
        CleanupSubscribers();
    }
}

void DsuServer::Impl::HandleMessage(const uint8_t* buf, int n, const struct sockaddr_in& src) {
    if (n < static_cast<int>(HEADER_LEN_FULL)) return;
    if (memcmp(buf, MAGIC_CLIENT, 4) != 0) return;
    if (R16(buf + 4) != PROTO_VER) return;

    uint16_t payLen = R16(buf + 6);
    if (n < static_cast<int>(HEADER_LEN + payLen)) return;
    if (R32(buf + CRC_OFFSET) != CrcZeroed(buf, HEADER_LEN + payLen)) return;

    uint32_t clientId = R32(buf + 12);
    uint32_t mtype    = R32(buf + 16);
    const uint8_t* pay = buf + HEADER_LEN_FULL;
    int plen = n - static_cast<int>(HEADER_LEN_FULL);

    switch (mtype) {
        case MSG_VERSION:
            SendVersion(src);
            break;
        case MSG_PORTS:
            if (plen >= 4) {
                uint32_t amt = R32(pay);
                if (amt > 4) amt = 4;
                for (uint32_t i = 0; i < amt && static_cast<int>(4 + i) <= plen; i++) {
                    uint8_t slot = pay[4 + i];
                    SendSlotInfo(src, slot);
                }
            }
            break;
        case MSG_DATA: {
            if (plen < 8) break;
            uint8_t reg = pay[0], slot = pay[1];
            bool want = (reg == 0) || (reg & 1 && slot < MAX_SLOTS) || (reg & 2);
            if (!want) break;

            std::lock_guard<std::mutex> lk(subMutex);
            if (subscribers.size() >= MAX_SUBS && subscribers.find(clientId) == subscribers.end())
                break;

            const bool wasEmpty = subscribers.empty();
            auto& sub = subscribers[clientId];
            sub.addr = src;
            sub.lastRequest = std::chrono::steady_clock::now();
            if (wasEmpty) {
                EventLog::Write("DSU: client connected (id 0x%08X, port %u)",
                                clientId, ntohs(src.sin_port));
            }
            break;
        }
    }
}

void DsuServer::Impl::SendVersion(const struct sockaddr_in& src) {
    uint8_t out[HEADER_LEN_FULL + 4] = {};
    WriteHeader(out, sizeof(out), serverId, MSG_VERSION);
    W16(out + HEADER_LEN_FULL, PROTO_VER);
    FinalizeCrc(out, sizeof(out));
    sendto(sock, reinterpret_cast<const char*>(out), sizeof(out), 0,
           reinterpret_cast<const struct sockaddr*>(&src), sizeof(src));
}

void DsuServer::Impl::SendSlotInfo(const struct sockaddr_in& src, uint8_t slot) {
    uint8_t out[HEADER_LEN_FULL + 12] = {};
    WriteHeader(out, sizeof(out), serverId, MSG_PORTS);

    bool conn = false;
    if (active.load() && slot < 4) {
        std::lock_guard<std::mutex> lk(stateMutex);
        conn = slotConnected[slot];
    }
    WriteCtrlHdr(out + HEADER_LEN_FULL, slot, conn);
    FinalizeCrc(out, sizeof(out));
    sendto(sock, reinterpret_cast<const char*>(out), sizeof(out), 0,
           reinterpret_cast<const struct sockaddr*>(&src), sizeof(src));
}

void DsuServer::Impl::CleanupSubscribers() {
    auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lk(subMutex);
    const bool wasEmpty = subscribers.empty();
    for (auto it = subscribers.begin(); it != subscribers.end();) {
        if (now - it->second.lastRequest >= SUB_TIMEOUT) {
            it = subscribers.erase(it);
        } else {
            ++it;
        }
    }
    if (!wasEmpty && subscribers.empty()) {
        EventLog::Write("DSU: all clients disconnected (idle)");
    }
}

void DsuServer::Impl::BroadcastData(int slot, uint64_t timestampUs,
                                   float ax, float ay, float az,
                                   float gx, float gy, float gz,
                                   const uint8_t* report, size_t len)
{
    std::lock_guard<std::mutex> lk(subMutex);
    if (subscribers.empty()) return;

    uint8_t uSlot = static_cast<uint8_t>(slot >= 0 && slot < 4 ? slot : 0);
    uint8_t out[HEADER_LEN_FULL + 80] = {}; // 100 bytes total

    WriteHeader(out, sizeof(out), serverId, MSG_DATA);
    WriteCtrlHdr(out + HEADER_LEN_FULL, uSlot, true);

    uint8_t* body = out + HEADER_LEN_FULL + CTRL_HDR_LEN;
    body[0] = 1; // connected

    // Extract triggers
    uint16_t ltrig = 0, rtrig = 0;
    if (len >= 10) {
        memcpy(&ltrig, report + 6, 2);
        memcpy(&rtrig, report + 8, 2);
    }
    uint8_t l2 = TriggerToU8(ltrig);
    uint8_t r2 = TriggerToU8(rtrig);
    bool l2d = (l2 >= 200);
    bool r2d = (r2 >= 200);

    // Extract buttons
    const uint8_t b0 = len > 2 ? report[2] : 0;
    const uint8_t b1 = len > 3 ? report[3] : 0;
    const uint8_t b2 = len > 4 ? report[4] : 0;

    const bool btnA     = (b0 & SteamController::BTN_A) != 0;
    const bool btnB     = (b0 & SteamController::BTN_B) != 0;
    const bool btnX     = (b0 & SteamController::BTN_X) != 0;
    const bool btnY     = (b0 & SteamController::BTN_Y) != 0;
    const bool btnMenu  = (b0 & SteamController::BTN_MENU) != 0;
    const bool btnView  = (b1 & SteamController::BTN_VIEW) != 0;
    const bool btnLS    = (b1 & SteamController::BTN_LS) != 0;
    const bool btnRS    = (b0 & SteamController::BTN_RS) != 0;
    const bool btnLB    = (b2 & SteamController::BTN_LB) != 0;
    const bool btnRB    = (b1 & SteamController::BTN_RB) != 0;
    const bool btnSteam = (b2 & SteamController::BTN_STEAM) != 0;
    const bool btnQAM   = (b0 & SteamController::BTN_QAM) != 0;
    const bool dpadUp   = (b1 & SteamController::BTN_DPAD_UP) != 0;
    const bool dpadDn   = (b1 & SteamController::BTN_DPAD_DN) != 0;
    const bool dpadLt   = (b1 & SteamController::BTN_DPAD_LT) != 0;
    const bool dpadRt   = (b1 & SteamController::BTN_DPAD_RT) != 0;

    // buttons1 at body[5]
    body[5] = static_cast<uint8_t>(
        (btnMenu ? 0x01 : 0) | (btnLS ? 0x02 : 0) | (btnRS ? 0x04 : 0) | (btnView ? 0x08 : 0) |
        (dpadUp ? 0x10 : 0) | (dpadRt ? 0x20 : 0) | (dpadDn ? 0x40 : 0) | (dpadLt ? 0x80 : 0));

    // buttons2 at body[6] (Cross=A, Circle=B, Square=X, Triangle=Y)
    body[6] = static_cast<uint8_t>(
        (l2d ? 0x01 : 0) | (r2d ? 0x02 : 0) | (btnLB ? 0x04 : 0) | (btnRB ? 0x08 : 0) |
        (btnY ? 0x10 : 0) | (btnB ? 0x20 : 0) | (btnA ? 0x40 : 0) | (btnX ? 0x80 : 0));

    body[7] = btnSteam ? 1 : 0;
    body[8] = btnQAM   ? 1 : 0;

    // Sticks
    int16_t lx = 0, ly = 0, rx = 0, ry = 0;
    if (len >= 18) {
        memcpy(&lx, report + 10, 2);
        memcpy(&ly, report + 12, 2);
        memcpy(&rx, report + 14, 2);
        memcpy(&ry, report + 16, 2);
    }
    body[9]  = StickToU8(lx);
    body[10] = StickToU8(ly);
    body[11] = StickToU8(rx);
    body[12] = StickToU8(ry);

    // Analog buttons
    auto fl = [](bool on) -> uint8_t { return on ? 255 : 0; };
    body[13] = fl(dpadLt);
    body[14] = fl(dpadDn);
    body[15] = fl(dpadRt);
    body[16] = fl(dpadUp);
    body[17] = fl(btnX);
    body[18] = fl(btnA);
    body[19] = fl(btnB);
    body[20] = fl(btnY);
    body[21] = fl(btnRB);
    body[22] = fl(btnLB);
    body[23] = r2;
    body[24] = l2;

    // Timestamp at body[37..44]
    memcpy(body + 37, &timestampUs, 8);

    // Motion data at body[45..68]
    float motion[6] = { ax, ay, az, gx, gy, gz };
    memcpy(body + 45, motion, 24);

    const size_t pcntOff = HEADER_LEN_FULL + CTRL_HDR_LEN + 1;

    for (auto& [id, sub] : subscribers) {
        sub.packetCounters[uSlot]++;
        W32(out + pcntOff, sub.packetCounters[uSlot]);
        FinalizeCrc(out, sizeof(out));
        sendto(sock, reinterpret_cast<const char*>(out), sizeof(out), 0,
               reinterpret_cast<const struct sockaddr*>(&sub.addr), sizeof(sub.addr));
    }
}

// ---------------------------------------------------------------------------
// DsuServer public API
// ---------------------------------------------------------------------------

DsuServer::DsuServer(uint16_t port)
    : m_impl(std::make_unique<Impl>())
{
    m_impl->port = port;
    m_impl->serverId = GenerateServerId();
}

DsuServer::~DsuServer() {
    Stop();
}

bool DsuServer::Start() {
    if (m_impl->running.load()) return true;

    static const bool s_wsa = [] {
        WSADATA d{};
        return WSAStartup(MAKEWORD(2, 2), &d) == 0;
    }();
    if (!s_wsa) {
        EventLog::Write("DSU: WSAStartup failed");
        return false;
    }

    m_impl->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_impl->sock == INVALID_SOCKET) {
        EventLog::Write("DSU: socket() creation failed: %d", WSAGetLastError());
        return false;
    }

    DWORD tv = 15; // 15 ms timeout
    setsockopt(m_impl->sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&tv), sizeof(tv));

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_impl->port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1

    if (bind(m_impl->sock, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        EventLog::Write("DSU: bind() failed on 127.0.0.1:%u (port in use? error %d)", m_impl->port, WSAGetLastError());
        closesocket(m_impl->sock);
        m_impl->sock = INVALID_SOCKET;
        return false;
    }

    m_impl->running = true;
    m_impl->thread = std::thread(&Impl::ServerLoop, m_impl.get());
    EventLog::Write("DSU: server listening on 127.0.0.1:%u (id 0x%08X)", m_impl->port, m_impl->serverId);
    return true;
}

void DsuServer::Stop() {
    m_impl->running = false;
    if (m_impl->thread.joinable()) {
        m_impl->thread.join();
    }
    if (m_impl->sock != INVALID_SOCKET) {
        closesocket(m_impl->sock);
        m_impl->sock = INVALID_SOCKET;
    }
}

void DsuServer::SetActive(bool active) {
    if (m_impl->active.load() != active) {
        m_impl->active.store(active);
        EventLog::Write("DSU: motion output is now %s", active ? "ACTIVE" : "PAUSED (native gyro mode)");
    }
}

bool DsuServer::IsActive() const {
    return m_impl->active.load();
}

bool DsuServer::IsRunning() const {
    return m_impl->running.load();
}

bool DsuServer::HasSubscribers() const {
    std::lock_guard<std::mutex> lk(m_impl->subMutex);
    return !m_impl->subscribers.empty();
}

void DsuServer::SetSlotConnected(int slot, bool connected) {
    if (slot >= 0 && slot < 4) {
        std::lock_guard<std::mutex> lk(m_impl->stateMutex);
        m_impl->slotConnected[slot] = connected;
    }
}

void DsuServer::PushReport(int slot, const uint8_t* report, size_t len) {
    if (!m_impl->active.load() || !m_impl->running.load() || m_impl->sock == INVALID_SOCKET) return;
    if (!HasSubscribers()) return;
    if (len < 46) return;

    int16_t rawAx = 0, rawAy = 0, rawAz = 0;
    int16_t rawGx = 0, rawGy = 0, rawGz = 0;
    memcpy(&rawAx, report + 34, 2);
    memcpy(&rawAy, report + 36, 2);
    memcpy(&rawAz, report + 38, 2);
    memcpy(&rawGx, report + 40, 2);
    memcpy(&rawGy, report + 42, 2);
    memcpy(&rawGz, report + 44, 2);

    float rawA[3] = {
        static_cast<float>(rawAx) / 16384.0f,
        static_cast<float>(rawAy) / 16384.0f,
        static_cast<float>(rawAz) / 16384.0f,
    };
    float rawG[3] = {
        static_cast<float>(rawGx) / 16.384f,
        static_cast<float>(rawGy) / 16.384f,
        static_cast<float>(rawGz) / 16.384f,
    };

    float ax = -rawA[0];
    float ay = -rawA[2];
    float az =  rawA[1];
    float gx =  rawG[0];
    float gy = -rawG[2];
    float gz =  rawG[1];

    uint64_t tsUs = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());

    m_impl->BroadcastData(slot, tsUs, ax, ay, az, gx, gy, gz, report, len);
}

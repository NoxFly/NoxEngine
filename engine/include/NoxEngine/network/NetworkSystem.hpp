// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace Nox {

    /// Network transport mode.
    enum class NetTransport : uint8_t {
        TCP,
        UDP
    };

    /// Network peer role.
    enum class NetRole : uint8_t {
        None,
        Server,
        Client
    };

    /// A network address (host + port).
    struct NetAddress {
        std::string host = "127.0.0.1";
        uint16_t port = 7777;
    };

    /// Packet of data to send/receive over the network.
    struct NetPacket {
        uint8_t channel = 0;
        std::vector<uint8_t> data;
    };

    /// Connection event info.
    struct NetConnectionEvent {
        uint32_t peerId = 0;
        NetAddress address;
    };

    /// Callback types for network events.
    using OnConnectFn    = std::function<void(const NetConnectionEvent&)>;
    using OnDisconnectFn = std::function<void(const NetConnectionEvent&)>;
    using OnReceiveFn    = std::function<void(uint32_t peerId, const NetPacket&)>;

    /// Low-level network system built on SDL3_net.
    /// Provides TCP/UDP server and client functionality.
    class NetworkSystem {
    public:
        NetworkSystem();
        ~NetworkSystem();

        NetworkSystem(const NetworkSystem&) = delete;
        NetworkSystem& operator=(const NetworkSystem&) = delete;
        NetworkSystem(NetworkSystem&&) = delete;
        NetworkSystem& operator=(NetworkSystem&&) = delete;

        /// Initialize SDL3_net subsystem.
        [[nodiscard]] bool init();

        /// Shutdown the network subsystem.
        void shutdown();

        /// Start a server listening on the given address.
        [[nodiscard]] bool startServer(const NetAddress& address, NetTransport transport = NetTransport::TCP);

        /// Connect to a remote server as a client.
        [[nodiscard]] bool connect(const NetAddress& address, NetTransport transport = NetTransport::TCP);

        /// Disconnect from the current server (client) or stop the server.
        void disconnect();

        /// Send a packet to a specific peer (server-side) or to the server (client-side).
        void send(uint32_t peerId, const NetPacket& packet);

        /// Broadcast a packet to all connected peers (server-side only).
        void broadcast(const NetPacket& packet);

        /// Process incoming network events. Call once per frame.
        void poll();

        /// Set event callbacks.
        void onConnect(OnConnectFn callback);
        void onDisconnect(OnDisconnectFn callback);
        void onReceive(OnReceiveFn callback);

        [[nodiscard]] NetRole role() const { return role_; }
        [[nodiscard]] bool isConnected() const { return connected_; }
        [[nodiscard]] bool isInitialized() const { return initialized_; }

    private:
        bool initialized_ = false;
        bool connected_ = false;
        NetRole role_ = NetRole::None;
        NetTransport transport_ = NetTransport::TCP;

        OnConnectFn onConnectCb_;
        OnDisconnectFn onDisconnectCb_;
        OnReceiveFn onReceiveCb_;

        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

} // namespace Nox

// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/network/NetworkSystem.hpp>
#include <NoxEngine/core/Logger.hpp>

#ifdef NOX_HAS_SDL3_NET
#include <SDL3_net/SDL_net.h>
#endif

namespace Nox {

    struct NetworkSystem::Impl {
#ifdef NOX_HAS_SDL3_NET
        NET_Server* server = nullptr;
        NET_StreamSocket* clientSocket = nullptr;
        uint32_t nextPeerId = 1;
        std::unordered_map<uint32_t, NET_StreamSocket*> peerMap;
#endif
    };

    NetworkSystem::NetworkSystem()
        : impl_(std::make_unique<Impl>())
    {}

    NetworkSystem::~NetworkSystem() {
        shutdown();
    }

    bool NetworkSystem::init() {
#ifdef NOX_HAS_SDL3_NET
        if (initialized_) { return true; }

        if (!NET_Init()) {
            NOX_LOG_ERROR("Failed to init SDL3_net: {}", SDL_GetError());
            return false;
        }

        initialized_ = true;
        NOX_LOG_INFO("Network system initialized (SDL3_net)");
        return true;
#else
        NOX_LOG_WARN("Network system not available (SDL3_net not linked)");
        return false;
#endif
    }

    void NetworkSystem::shutdown() {
#ifdef NOX_HAS_SDL3_NET
        if (!initialized_) { return; }
        disconnect();
        NET_Quit();
        initialized_ = false;
        NOX_LOG_INFO("Network system shut down");
#endif
    }

    bool NetworkSystem::startServer([[maybe_unused]] const NetAddress& address,
                                     [[maybe_unused]] NetTransport transport) {
#ifdef NOX_HAS_SDL3_NET
        if (!initialized_) { return false; }

        NET_Address* resolved = NET_ResolveHostname(address.host.c_str());
        if (!resolved) {
            NOX_LOG_ERROR("Failed to resolve hostname '{}': {}", address.host, SDL_GetError());
            return false;
        }

        if (NET_WaitUntilResolved(resolved, 5000) < 0) {
            NOX_LOG_ERROR("Hostname resolution timed out for '{}'", address.host);
            NET_UnrefAddress(resolved);
            return false;
        }

        impl_->server = NET_CreateServer(resolved, address.port);
        NET_UnrefAddress(resolved);

        if (!impl_->server) {
            NOX_LOG_ERROR("Failed to create server on port {}: {}", address.port, SDL_GetError());
            return false;
        }

        role_ = NetRole::Server;
        connected_ = true;
        transport_ = transport;
        NOX_LOG_INFO("Server started on {}:{}", address.host, address.port);
        return true;
#else
        return false;
#endif
    }

    bool NetworkSystem::connect([[maybe_unused]] const NetAddress& address,
                                 [[maybe_unused]] NetTransport transport) {
#ifdef NOX_HAS_SDL3_NET
        if (!initialized_) { return false; }

        NET_Address* resolved = NET_ResolveHostname(address.host.c_str());
        if (!resolved) {
            NOX_LOG_ERROR("Failed to resolve hostname '{}': {}", address.host, SDL_GetError());
            return false;
        }

        if (NET_WaitUntilResolved(resolved, 5000) < 0) {
            NOX_LOG_ERROR("Hostname resolution timed out for '{}'", address.host);
            NET_UnrefAddress(resolved);
            return false;
        }

        impl_->clientSocket = NET_CreateClient(resolved, address.port);
        NET_UnrefAddress(resolved);

        if (!impl_->clientSocket) {
            NOX_LOG_ERROR("Failed to connect to {}:{}: {}", address.host, address.port, SDL_GetError());
            return false;
        }

        // Wait for connection to establish
        if (NET_WaitUntilConnected(impl_->clientSocket, 5000) < 0) {
            NOX_LOG_ERROR("Connection to {}:{} failed", address.host, address.port);
            NET_DestroyStreamSocket(impl_->clientSocket);
            impl_->clientSocket = nullptr;
            return false;
        }

        role_ = NetRole::Client;
        connected_ = true;
        transport_ = transport;
        NOX_LOG_INFO("Connected to server {}:{}", address.host, address.port);

        if (onConnectCb_) {
            onConnectCb_({ 0, address });
        }

        return true;
#else
        return false;
#endif
    }

    void NetworkSystem::disconnect() {
#ifdef NOX_HAS_SDL3_NET
        if (!connected_) { return; }

        if (role_ == NetRole::Server) {
            for (auto& [id, sock] : impl_->peerMap) {
                NET_DestroyStreamSocket(sock);
            }
            impl_->peerMap.clear();
            if (impl_->server) {
                NET_DestroyServer(impl_->server);
                impl_->server = nullptr;
            }
        }
        else if (role_ == NetRole::Client) {
            if (impl_->clientSocket) {
                NET_DestroyStreamSocket(impl_->clientSocket);
                impl_->clientSocket = nullptr;
            }
        }

        connected_ = false;
        role_ = NetRole::None;
#endif
    }

    void NetworkSystem::send([[maybe_unused]] uint32_t peerId,
                              [[maybe_unused]] const NetPacket& packet) {
#ifdef NOX_HAS_SDL3_NET
        if (!connected_) { return; }

        NET_StreamSocket* sock = nullptr;
        if (role_ == NetRole::Client) {
            sock = impl_->clientSocket;
        }
        else {
            auto it = impl_->peerMap.find(peerId);
            if (it != impl_->peerMap.end()) {
                sock = it->second;
            }
        }

        if (sock && !packet.data.empty()) {
            auto len = static_cast<uint32_t>(packet.data.size());
            NET_WriteToStreamSocket(sock, &len, sizeof(len));
            NET_WriteToStreamSocket(sock, packet.data.data(), static_cast<int>(packet.data.size()));
        }
#endif
    }

    void NetworkSystem::broadcast([[maybe_unused]] const NetPacket& packet) {
#ifdef NOX_HAS_SDL3_NET
        if (role_ != NetRole::Server || !connected_) { return; }
        for (auto& [id, sock] : impl_->peerMap) {
            send(id, packet);
        }
#endif
    }

    void NetworkSystem::poll() {
#ifdef NOX_HAS_SDL3_NET
        if (!connected_) { return; }

        if (role_ == NetRole::Server && impl_->server) {
            // Accept new connections
            NET_StreamSocket* newSock = nullptr;
            while (NET_AcceptClient(impl_->server, &newSock) && newSock) {
                uint32_t peerId = impl_->nextPeerId++;
                impl_->peerMap[peerId] = newSock;

                if (onConnectCb_) {
                    onConnectCb_({ peerId, {} });
                }
                newSock = nullptr;
            }

            // Read from connected peers
            for (auto it = impl_->peerMap.begin(); it != impl_->peerMap.end(); ) {
                uint32_t peerId = it->first;
                NET_StreamSocket* sock = it->second;

                // Check connection status
                auto status = NET_GetConnectionStatus(sock);
                if (status == NET_FAILURE) {
                    if (onDisconnectCb_) {
                        onDisconnectCb_({ peerId, {} });
                    }
                    NET_DestroyStreamSocket(sock);
                    it = impl_->peerMap.erase(it);
                    continue;
                }

                // Try to read data
                uint32_t len = 0;
                int result = NET_ReadFromStreamSocket(sock, &len, sizeof(len));
                if (result == sizeof(len) && len > 0 && len < 65536) {
                    NetPacket packet;
                    packet.data.resize(len);
                    NET_ReadFromStreamSocket(sock, packet.data.data(), static_cast<int>(len));
                    if (onReceiveCb_) {
                        onReceiveCb_(peerId, packet);
                    }
                }
                ++it;
            }
        }
        else if (role_ == NetRole::Client && impl_->clientSocket) {
            auto status = NET_GetConnectionStatus(impl_->clientSocket);
            if (status == NET_FAILURE) {
                if (onDisconnectCb_) {
                    onDisconnectCb_({ 0, {} });
                }
                disconnect();
                return;
            }

            uint32_t len = 0;
            int result = NET_ReadFromStreamSocket(impl_->clientSocket, &len, sizeof(len));
            if (result == sizeof(len) && len > 0 && len < 65536) {
                NetPacket packet;
                packet.data.resize(len);
                NET_ReadFromStreamSocket(impl_->clientSocket, packet.data.data(), static_cast<int>(len));
                if (onReceiveCb_) {
                    onReceiveCb_(0, packet);
                }
            }
        }
#endif
    }

    void NetworkSystem::onConnect(OnConnectFn callback) {
        onConnectCb_ = std::move(callback);
    }

    void NetworkSystem::onDisconnect(OnDisconnectFn callback) {
        onDisconnectCb_ = std::move(callback);
    }

    void NetworkSystem::onReceive(OnReceiveFn callback) {
        onReceiveCb_ = std::move(callback);
    }

} // namespace Nox

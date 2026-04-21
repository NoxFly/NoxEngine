// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/network/NetworkSystem.hpp>

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Nox {

    /// Session state.
    enum class SessionState : uint8_t {
        None,
        Lobby,
        InGame,
        Ended
    };

    /// Represents a player in a network session.
    struct NetPlayer {
        uint32_t peerId = 0;
        std::string name;
        bool ready = false;
    };

    /// High-level network session / lobby manager.
    /// Wraps NetworkSystem with session semantics (lobby, ready-check, game start).
    class NetworkSession {
    public:
        NetworkSession() = default;
        ~NetworkSession() = default;

        NetworkSession(const NetworkSession&) = delete;
        NetworkSession& operator=(const NetworkSession&) = delete;

        /// Host a new session.
        [[nodiscard]] bool host(NetworkSystem& net, const NetAddress& address,
                                 std::string_view sessionName, uint32_t maxPlayers = 8);

        /// Join an existing session.
        [[nodiscard]] bool join(NetworkSystem& net, const NetAddress& address,
                                 std::string_view playerName);

        /// Leave/destroy the current session.
        void leave(NetworkSystem& net);

        /// Mark the local player as ready.
        void setReady(bool ready);

        /// Check if all players are ready (host only).
        [[nodiscard]] bool allPlayersReady() const;

        /// Start the game (host only, transitions from Lobby to InGame).
        void startGame();

        /// End the game session.
        void endGame();

        /// Process session-level events from raw packets.
        void processPacket(uint32_t peerId, const NetPacket& packet);

        /// Handle player connections.
        void onPlayerConnected(const NetConnectionEvent& event);
        void onPlayerDisconnected(const NetConnectionEvent& event);

        [[nodiscard]] SessionState state() const { return state_; }
        [[nodiscard]] const std::unordered_map<uint32_t, NetPlayer>& players() const { return players_; }
        [[nodiscard]] const std::string& sessionName() const { return sessionName_; }
        [[nodiscard]] uint32_t maxPlayers() const { return maxPlayers_; }

    private:
        SessionState state_ = SessionState::None;
        std::string sessionName_;
        uint32_t maxPlayers_ = 8;
        std::unordered_map<uint32_t, NetPlayer> players_;
        bool isHost_ = false;
    };

} // namespace Nox

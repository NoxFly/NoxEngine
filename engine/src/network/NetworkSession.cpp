// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/network/NetworkSession.hpp>
#include <NoxEngine/core/Logger.hpp>

namespace Nox {

    bool NetworkSession::host(NetworkSystem& net, const NetAddress& address,
                               std::string_view sessionName, uint32_t maxPlayers) {
        if (!net.startServer(address)) { return false; }

        sessionName_ = sessionName;
        maxPlayers_ = maxPlayers;
        isHost_ = true;
        state_ = SessionState::Lobby;

        // Register self as host player
        players_[0] = { 0, "Host", false };

        NOX_LOG_INFO("Hosting session '{}' (max {} players)", sessionName_, maxPlayers_);
        return true;
    }

    bool NetworkSession::join(NetworkSystem& net, const NetAddress& address,
                               std::string_view playerName) {
        if (!net.connect(address)) { return false; }

        isHost_ = false;
        state_ = SessionState::Lobby;
        players_[0] = { 0, std::string(playerName), false };

        NOX_LOG_INFO("Joined session as '{}'", playerName);
        return true;
    }

    void NetworkSession::leave(NetworkSystem& net) {
        net.disconnect();
        state_ = SessionState::None;
        players_.clear();
        isHost_ = false;
        sessionName_.clear();
    }

    void NetworkSession::setReady(bool ready) {
        auto it = players_.find(0);
        if (it != players_.end()) {
            it->second.ready = ready;
        }
    }

    bool NetworkSession::allPlayersReady() const {
        for (const auto& [id, player] : players_) {
            if (!player.ready) { return false; }
        }
        return !players_.empty();
    }

    void NetworkSession::startGame() {
        if (!isHost_ || state_ != SessionState::Lobby) { return; }
        state_ = SessionState::InGame;
        NOX_LOG_INFO("Game started for session '{}'", sessionName_);
    }

    void NetworkSession::endGame() {
        state_ = SessionState::Ended;
        NOX_LOG_INFO("Game ended for session '{}'", sessionName_);
    }

    void NetworkSession::processPacket([[maybe_unused]] uint32_t peerId,
                                        [[maybe_unused]] const NetPacket& packet) {
        // Protocol-level packet processing (to be extended with game-specific messages)
        // For now, this is a placeholder for the networking foundation
    }

    void NetworkSession::onPlayerConnected(const NetConnectionEvent& event) {
        if (players_.size() >= maxPlayers_) {
            NOX_LOG_WARN("Session full, rejecting peer {}", event.peerId);
            return;
        }

        players_[event.peerId] = { event.peerId, "Player" + std::to_string(event.peerId), false };
        NOX_LOG_INFO("Player {} connected to session (total: {})",
                     event.peerId, players_.size());
    }

    void NetworkSession::onPlayerDisconnected(const NetConnectionEvent& event) {
        players_.erase(event.peerId);
        NOX_LOG_INFO("Player {} disconnected from session (total: {})",
                     event.peerId, players_.size());
    }

} // namespace Nox

// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include "Level.hpp"

/// Demonstrates network system: session management, entity replication, snapshot interpolation.
class LevelNetworking : public Level {
public:
    LevelNetworking() : Level("Networking") {}

    void setup(Nox::Engine& engine) override;
    void teardown(Nox::Engine& engine) override;
    void update(Nox::Engine& engine, float dt) override;
    void drawUI() override;

    Nox::Scene3D& scene() override { return scene_; }
    Nox::PerspectiveCamera& camera() override { return camera_; }

private:
    Nox::Scene3D scene_;
    Nox::PerspectiveCamera camera_{ 45.0f, 1.77f, 0.1f, 1000.0f };
    Nox::OrbitCameraController orbitCtrl_{ camera_ };

    Nox::NetworkSystem netSystem_;
    Nox::NetworkSession session_;
    Nox::EntityReplication replication_;
    bool netInitialized_ = false;

    // Simulated entities
    std::shared_ptr<Nox::Mesh> entity1_;
    std::shared_ptr<Nox::Mesh> entity2_;
    float gameTime_ = 0.0f;
    float snapshotInterval_ = 0.1f; // 10 Hz
    float snapshotTimer_ = 0.0f;
    float interpolationDelay_ = 0.1f;

    // Replication demo state
    std::string netStatus_ = "Not initialized";
};

// ── Implementation ─────────────────────────────────────────────────

#ifdef NOX_HAS_IMGUI
#include <imgui.h>
#endif

inline void LevelNetworking::setup([[maybe_unused]] Nox::Engine& engine) {
    using namespace Nox;

    camera_.setPosition(5.0f, 5.0f, 8.0f);
    camera_.lookAt(0.0f, 0.0f, 0.0f);

    // Lights
    auto sun = std::make_shared<DirectionalLight>(Color::White, 1.0f);
    sun->setDirection(-1.0f, -1.0f, -0.5f);
    scene_.add(sun);

    auto ambient = std::make_shared<AmbientLight>(Color::White, 0.15f);
    scene_.add(ambient);

    // Floor
    auto floorGeom = Geometry::plane(20.0f, 20.0f);
    auto floorMat = Material::standard();
    floorMat->setColor(Color(0.3f, 0.3f, 0.35f));
    scene_.add(std::make_shared<Mesh>(floorGeom, floorMat));

    // Simulated network entities
    auto geom1 = Geometry::box(0.5f, 0.5f, 0.5f);
    auto mat1 = Material::standard();
    mat1->setColor(Color(0.9f, 0.2f, 0.2f));
    entity1_ = std::make_shared<Mesh>(geom1, mat1);
    entity1_->setPosition(0.0f, 0.25f, 0.0f);
    scene_.add(entity1_);

    auto geom2 = Geometry::sphere(0.3f, 16, 8);
    auto mat2 = Material::standard();
    mat2->setColor(Color(0.2f, 0.2f, 0.9f));
    entity2_ = std::make_shared<Mesh>(geom2, mat2);
    entity2_->setPosition(3.0f, 0.3f, 0.0f);
    scene_.add(entity2_);

    // Register entities for replication
    replication_.registerEntity(1);
    replication_.registerEntity(2);
    replication_.setInterpolationDelay(interpolationDelay_);

    // Initialize network subsystem
    netInitialized_ = netSystem_.init();
    if (netInitialized_) {
        netStatus_ = "Initialized (no connection)";
    }
    else {
        netStatus_ = "SDL3_net not available";
    }
}

inline void LevelNetworking::teardown([[maybe_unused]] Nox::Engine& engine) {
    session_.leave(netSystem_);
    netSystem_.shutdown();
    entity1_.reset();
    entity2_.reset();
    scene_ = Nox::Scene3D();
    netInitialized_ = false;
}

inline void LevelNetworking::update(Nox::Engine& engine, float dt) {
    orbitCtrl_.update(engine.input(), dt);
    gameTime_ += dt;

    // Simulate entity movement (as if driven by network)
    float x1 = std::cos(gameTime_) * 3.0f;
    float z1 = std::sin(gameTime_) * 3.0f;
    entity1_->setPosition(x1, 0.25f, z1);

    float x2 = std::sin(gameTime_ * 0.7f) * 2.0f;
    float z2 = std::cos(gameTime_ * 0.7f) * 4.0f;
    entity2_->setPosition(x2, 0.3f, z2);

    // Snapshot & replication simulation
    snapshotTimer_ += dt;
    if (snapshotTimer_ >= snapshotInterval_) {
        snapshotTimer_ = 0.0f;

        // Update entity states
        Nox::EntitySnapshot snap1;
        snap1.entityId = 1;
        snap1.position = { x1, 0.25f, z1 };
        snap1.timestamp = gameTime_;
        replication_.updateEntityState(1, snap1);

        Nox::EntitySnapshot snap2;
        snap2.entityId = 2;
        snap2.position = { x2, 0.3f, z2 };
        snap2.timestamp = gameTime_;
        replication_.updateEntityState(2, snap2);

        // Take and apply snapshot (simulate send/receive)
        auto worldSnap = replication_.takeSnapshot(gameTime_);
        replication_.applySnapshot(worldSnap);
    }

    // Poll network if initialized
    if (netInitialized_) {
        netSystem_.poll();
    }
}

inline void LevelNetworking::drawUI() {
#ifdef NOX_HAS_IMGUI
    ImGui::Text("Networking Demo");
    ImGui::Separator();

    ImGui::Text("Network: %s", netStatus_.c_str());
    ImGui::Text("Role: %s",
                netSystem_.role() == Nox::NetRole::Server ? "Server" :
                netSystem_.role() == Nox::NetRole::Client ? "Client" : "None");

    ImGui::Separator();
    ImGui::Text("Entity Replication");
    ImGui::SliderFloat("Snapshot Rate (Hz)", &snapshotInterval_, 0.01f, 1.0f, "%.3f s");
    ImGui::SliderFloat("Interp Delay (s)", &interpolationDelay_, 0.0f, 0.5f);

    if (ImGui::Button("Update Interp Delay")) {
        replication_.setInterpolationDelay(interpolationDelay_);
    }

    auto* s1 = replication_.getEntityState(1);
    auto* s2 = replication_.getEntityState(2);
    if (s1) {
        ImGui::Text("Entity 1: (%.1f, %.1f, %.1f)", s1->position.x, s1->position.y, s1->position.z);
    }
    if (s2) {
        ImGui::Text("Entity 2: (%.1f, %.1f, %.1f)", s2->position.x, s2->position.y, s2->position.z);
    }

    ImGui::Separator();
    ImGui::Text("Session");
    ImGui::Text("State: %s",
                session_.state() == Nox::SessionState::Lobby ? "Lobby" :
                session_.state() == Nox::SessionState::InGame ? "In Game" :
                session_.state() == Nox::SessionState::Ended ? "Ended" : "None");
    ImGui::Text("Players: %zu", session_.players().size());

    if (netInitialized_ && session_.state() == Nox::SessionState::None) {
        if (ImGui::Button("Host Session (port 7777)")) {
            if (session_.host(netSystem_, { "127.0.0.1", 7777 }, "TestSession", 4)) {
                netStatus_ = "Hosting on port 7777";
            }
        }
    }
#endif
}

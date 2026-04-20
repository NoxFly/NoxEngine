// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include "Level.hpp"
#include <NoxEngine/physics/PhysicsWorld.hpp>

#include <vector>

class LevelPhysicsBasic : public Level {
public:
    LevelPhysicsBasic() : Level("Physics — Basic") {}

    void setup(Nox::Engine& engine) override;
    void teardown(Nox::Engine& engine) override;
    void update(Nox::Engine& engine, float dt) override;
    void drawUI() override;

    Nox::Scene3D& scene() override { return scene_; }
    Nox::PerspectiveCamera& camera() override { return camera_; }

private:
    void spawnBox(const Nox::Math::Vec3& pos);

    Nox::Scene3D scene_;
    Nox::PerspectiveCamera camera_{ 45.0f, 1.77f, 0.1f, 1000.0f };
    Nox::OrbitCameraController orbitCtrl_{ camera_ };
    Nox::PhysicsWorld physics_;

    struct PhysicsObject {
        std::shared_ptr<Nox::Mesh> mesh;
        Nox::BodyHandle body;
    };

    std::vector<PhysicsObject> objects_;
    bool paused_ = false;
};

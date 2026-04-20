// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include "Level.hpp"

class LevelGeometry : public Level {
public:
    LevelGeometry() : Level("Geometry & PBR") {}

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

    std::shared_ptr<Nox::Mesh> boxMesh_;
    std::shared_ptr<Nox::Mesh> sphereMesh_;
    std::shared_ptr<Nox::Mesh> emissiveCube_;
    float timer_ = 0.0f;
};

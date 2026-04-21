// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include "Level.hpp"
#include <NoxEngine/renderer/Sky.hpp>
#include <NoxEngine/renderer/Billboard.hpp>
#include <NoxEngine/scene/ObjectPicker.hpp>

class LevelSky : public Level {
public:
    LevelSky() : Level("Sky & Picking") {}

    void setup(Nox::Engine& engine) override;
    void teardown(Nox::Engine& engine) override;
    void update(Nox::Engine& engine, float dt) override;
    void drawUI() override;

    Nox::Scene3D& scene() override { return scene_; }
    Nox::PerspectiveCamera& camera() override { return camera_; }

private:
    Nox::Scene3D scene_;
    Nox::PerspectiveCamera camera_{ 45.0f, 1.77f, 0.1f, 2000.0f };
    Nox::OrbitCameraController orbitCtrl_{ camera_ };

    Nox::Sky sky_;
    Nox::ObjectPicker picker_;
    Nox::TextRenderer textRenderer_;

    std::shared_ptr<Nox::Mesh> selectedMesh_;
    float timeOfDay_ = 0.3f; // 0..1 maps to sun angle
    Nox::Engine* engine_ = nullptr;
};

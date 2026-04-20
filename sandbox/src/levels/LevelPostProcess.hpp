// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include "Level.hpp"

class LevelPostProcess : public Level {
public:
    LevelPostProcess() : Level("Post-Processing") {}

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

    Nox::Engine* engine_ = nullptr;
    bool fxaaEnabled_ = true;
    bool bloomEnabled_ = true;
    bool ssaoEnabled_ = true;
    float exposure_ = 1.0f;
    float bloomThreshold_ = 1.0f;
    float bloomIntensity_ = 0.5f;
};

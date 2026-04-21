// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include "Level.hpp"

/// Demonstrates cinematic post-processing effects:
/// Film Grain, Vignette, Chromatic Aberration — layered on top of the
/// existing SSAO + Bloom + FXAA pipeline.
class LevelCinematic : public Level {
public:
    LevelCinematic() : Level("Cinematic FX") {}

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

    // Effect toggles
    bool grainEnabled_    = true;
    bool vignetteEnabled_ = true;
    bool chromaEnabled_   = true;
    bool bloomEnabled_    = true;

    // Effect parameters
    float grainIntensity_ = 0.08f;
    float vignetteIntensity_  = 0.8f;
    float vignetteSmoothness_ = 0.4f;
    float chromaIntensity_    = 0.003f;
    float exposure_           = 1.2f;
};

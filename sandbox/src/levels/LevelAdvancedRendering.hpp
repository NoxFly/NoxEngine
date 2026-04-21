// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include "Level.hpp"

#include <NoxEngine/renderer/GBuffer.hpp>
#include <NoxEngine/renderer/CascadedShadowMap.hpp>
#include <NoxEngine/renderer/ClusteredLighting.hpp>
#include <NoxEngine/renderer/Decal.hpp>
#include <NoxEngine/renderer/effects/SSREffect.hpp>
#include <NoxEngine/renderer/effects/TAAEffect.hpp>
#include <NoxEngine/renderer/effects/VolumetricFogEffect.hpp>

/// Demonstrates v1.1 advanced rendering features:
/// G-Buffer, clustered lighting, CSM, SSR, TAA, volumetric fog, decals.
class LevelAdvancedRendering : public Level {
public:
    LevelAdvancedRendering() : Level("Advanced Rendering") {}

    void setup(Nox::Engine& engine) override;
    void teardown(Nox::Engine& engine) override;
    void update(Nox::Engine& engine, float dt) override;
    void drawUI() override;

    Nox::Scene3D& scene() override { return scene_; }
    Nox::PerspectiveCamera& camera() override { return camera_; }

private:
    Nox::Scene3D scene_;
    Nox::PerspectiveCamera camera_{ 45.0f, 1.77f, 0.1f, 500.0f };
    Nox::OrbitCameraController orbitCtrl_{ camera_ };

    Nox::GBuffer gBuffer_;
    Nox::CascadedShadowMap csm_;
    Nox::ClusteredLighting clusteredLighting_;
    Nox::DecalRenderer decalRenderer_;

    // Post-process effects (v1.1) — raw pointers for UI access after ownership transfer
    Nox::SSREffect* ssrEffect_           = nullptr;
    Nox::TAAEffect* taaEffect_           = nullptr;
    Nox::VolumetricFogEffect* fogEffect_ = nullptr;

    std::vector<std::shared_ptr<Nox::Decal>> decals_;

    // Settings
    bool showGBuffer_     = false;
    bool enableCSM_       = true;
    bool enableClustered_ = true;
    bool enableSSR_       = true;
    bool enableTAA_       = true;
    bool enableFog_       = true;
    bool enableDecals_    = true;
    int  gBufferDisplay_  = 0; // 0=final, 1=position, 2=normal, 3=albedo, 4=emissive

    Nox::Engine* engine_ = nullptr;
};

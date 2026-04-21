// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include "../Level.hpp"

#include <NoxEngine/scene/Terrain.hpp>
#include <NoxEngine/renderer/InstancedVegetation.hpp>
#include <NoxEngine/renderer/ImposterSystem.hpp>
#include <NoxEngine/renderer/OcclusionCulling.hpp>

/// Demonstrates v1.2 terrain & large world features:
/// Heightmap terrain with LOD, splatting, chunk streaming,
/// instanced vegetation, imposters, occlusion culling.
class LevelTerrain : public Level {
public:
    LevelTerrain() : Level("Terrain") {}

    void setup(Nox::Engine& engine) override;
    void teardown(Nox::Engine& engine) override;
    void update(Nox::Engine& engine, float dt) override;
    void drawUI() override;

    Nox::Scene3D& scene() override { return scene_; }
    Nox::PerspectiveCamera& camera() override { return camera_; }

private:
    Nox::Scene3D scene_;
    Nox::PerspectiveCamera camera_{ 45.0f, 1.77f, 0.1f, 2000.0f };
    Nox::OrbitCameraController orbitController_{ camera_ };

    Nox::Terrain terrain_;
    Nox::InstancedVegetation vegetation_;
    Nox::ImposterSystem imposters_;
    Nox::OcclusionCulling occlusionCulling_;

    bool showWireframe_ = false;
    bool enableOcclusion_ = true;
    bool enableVegetation_ = true;
    int  terrainSeed_ = 42;
};

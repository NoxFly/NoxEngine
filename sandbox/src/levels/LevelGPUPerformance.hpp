// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include "Level.hpp"

class LevelGPUPerformance : public Level {
public:
    LevelGPUPerformance() : Level("GPU Performance") {}

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

    std::shared_ptr<Nox::InstancedMesh> instancedCubes_;
    Nox::GPUParticleSystem particles_;
    float timer_ = 0.0f;
    int instanceCount_ = 10000;
};

// ── Implementation ─────────────────────────────────────────────────

#ifdef NOX_HAS_IMGUI
#include <imgui.h>
#endif

inline void LevelGPUPerformance::setup(Nox::Engine& engine) {
    using namespace Nox;

    // Camera
    camera_.setPosition(20.0f, 15.0f, 20.0f);
    camera_.lookAt(0.0f, 0.0f, 0.0f);

    // Lights
    auto sun = std::make_shared<DirectionalLight>(Color::White, 1.2f);
    sun->setDirection(-1.0f, -1.0f, -0.5f);
    scene_.add(sun);

    auto ambient = std::make_shared<AmbientLight>(Color::White, 0.1f);
    scene_.add(ambient);

    // Floor
    auto floorGeom = Geometry::plane(50.0f, 50.0f);
    auto floorMat = Material::standard();
    floorMat->setColor(Color(0.3f, 0.3f, 0.3f));
    auto floor = std::make_shared<Mesh>(floorGeom, floorMat);
    scene_.add(floor);

    // ── GPU Instancing demo: thousands of cubes ────────────────────
    auto cubeGeom = Geometry::box(0.3f, 0.3f, 0.3f);
    auto cubeMat = Material::standard();
    cubeMat->setColor(Color(0.8f, 0.2f, 0.1f));
    cubeMat->setRoughness(0.4f);
    cubeMat->setMetallic(0.6f);

    std::vector<Math::Mat4> transforms;
    transforms.reserve(static_cast<size_t>(instanceCount_));

    int gridSize = static_cast<int>(std::sqrt(static_cast<float>(instanceCount_)));
    float spacing = 0.6f;
    float offset = static_cast<float>(gridSize) * spacing * 0.5f;

    for (int z = 0; z < gridSize; ++z) {
        for (int x = 0; x < gridSize; ++x) {
            Math::Mat4 t = glm::translate(Math::Mat4(1.0f),
                Math::Vec3(
                    static_cast<float>(x) * spacing - offset,
                    0.15f,
                    static_cast<float>(z) * spacing - offset
                ));
            transforms.push_back(t);
        }
    }

    instancedCubes_ = scene_.addInstanced(cubeGeom, cubeMat, std::move(transforms));

    // ── GPU Particle system demo ───────────────────────────────────
    GPUParticleSystem::Config particleConfig;
    particleConfig.maxParticles = 500000;
    particleConfig.emitRate = 50000.0f;
    particleConfig.lifeMin = 1.0f;
    particleConfig.lifeMax = 3.0f;
    particleConfig.speedMin = 2.0f;
    particleConfig.speedMax = 8.0f;
    particleConfig.gravity = { 0.0f, -5.0f, 0.0f };
    particleConfig.emitDirection = { 0.0f, 1.0f, 0.0f };
    particleConfig.emitSpread = 0.3f;
    particleConfig.sizeStart = 0.03f;
    particleConfig.sizeEnd = 0.0f;
    particleConfig.colorStart = { 1.0f, 0.5f, 0.1f, 1.0f };
    particleConfig.colorEnd = { 1.0f, 0.0f, 0.0f, 0.0f };

    particles_.setPosition(0.0f, 0.5f, 0.0f);
    particles_.init(particleConfig);
    scene_.add(std::shared_ptr<SceneObject>(
        std::shared_ptr<void>{}, &particles_)); // non-owning — particles_ is stack-owned

    [[maybe_unused]] auto& ctx = engine.renderContext();
}

inline void LevelGPUPerformance::teardown([[maybe_unused]] Nox::Engine& engine) {
    instancedCubes_.reset();
    scene_ = Nox::Scene3D();
}

inline void LevelGPUPerformance::update(Nox::Engine& engine, float dt) {
    timer_ += dt;
    orbitCtrl_.update(engine.input(), dt);
    particles_.update(dt);
}

inline void LevelGPUPerformance::drawUI() {
#ifdef NOX_HAS_IMGUI
    ImGui::Text("GPU Instancing: %d cubes", instanceCount_);
    ImGui::Text("GPU Particles: %d max", particles_.config().maxParticles);
    ImGui::Separator();
    ImGui::Text("All rendered with single draw calls");
#endif
}

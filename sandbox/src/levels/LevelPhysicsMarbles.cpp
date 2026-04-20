// Copyright (c) 2026 NoxFly — AGPL-3.0

#include "levels/LevelPhysicsMarbles.hpp"

#ifdef NOX_HAS_IMGUI
#include <imgui.h>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cmath>
#include <random>

using namespace Nox;

void LevelPhysicsMarbles::setup(Engine& engine) {
    camera_.setAspect(engine.aspect());
    camera_.setPosition(6.0f, 8.0f, 6.0f);
    camera_.lookAt(0.0f, 1.0f, 0.0f);
    orbitCtrl_.syncFromCamera();

    physics_.init();
    spawned_ = 0;
    spawnTimer_ = 0.0f;

    createContainer();

    // Lights
    auto sun = std::make_shared<DirectionalLight>(Color::White, 0.8f);
    sun->setDirection(-0.5f, -1.0f, -0.3f);
    scene_.add(sun);

    auto ambient = std::make_shared<AmbientLight>(Color::White, 0.15f);
    scene_.add(ambient);

    auto point = std::make_shared<PointLight>(Color(1.0f, 0.9f, 0.7f), 2.0f);
    point->setPosition(0.0f, 5.0f, 0.0f);
    point->setRange(15.0f);
    scene_.add(point);
}

void LevelPhysicsMarbles::createContainer() {
    constexpr float wallThick = 0.2f;
    constexpr float halfW = 2.0f;
    constexpr float wallH = 2.5f;

    auto wallMat = Material::standard();
    wallMat->setColor(Color(0.5f, 0.5f, 0.55f));
    wallMat->setRoughness(0.7f);
    wallMat->setMetallic(0.1f);

    // Floor
    {
        auto mesh = std::make_shared<Mesh>(
            Geometry::box(halfW * 2.0f, wallThick, halfW * 2.0f), wallMat);
        mesh->setPosition(0.0f, -wallThick * 0.5f, 0.0f);
        scene_.add(mesh);

        RigidBodyDesc desc;
        desc.shape = BoxShape{ Math::Vec3(halfW, wallThick * 0.5f, halfW) };
        desc.motionType = MotionType::Static;
        desc.position = Math::Vec3(0.0f, -wallThick * 0.5f, 0.0f);
        desc.friction = 0.8f;
        walls_.push_back(physics_.createBody(desc));
    }

    // 4 walls
    struct WallDef {
        float px, py, pz;
        float hx, hy, hz;
    };

    WallDef wallDefs[] = {
        {  0.0f,     wallH * 0.5f,  halfW, halfW,          wallH * 0.5f, wallThick * 0.5f },
        {  0.0f,     wallH * 0.5f, -halfW, halfW,          wallH * 0.5f, wallThick * 0.5f },
        {  halfW,    wallH * 0.5f,  0.0f,  wallThick * 0.5f, wallH * 0.5f, halfW },
        { -halfW,    wallH * 0.5f,  0.0f,  wallThick * 0.5f, wallH * 0.5f, halfW },
    };

    for (const auto& w : wallDefs) {
        auto mesh = std::make_shared<Mesh>(
            Geometry::box(w.hx * 2.0f, w.hy * 2.0f, w.hz * 2.0f), wallMat);
        mesh->setPosition(w.px, w.py, w.pz);
        scene_.add(mesh);

        RigidBodyDesc desc;
        desc.shape = BoxShape{ Math::Vec3(w.hx, w.hy, w.hz) };
        desc.motionType = MotionType::Static;
        desc.position = Math::Vec3(w.px, w.py, w.pz);
        desc.friction = 0.5f;
        walls_.push_back(physics_.createBody(desc));
    }
}

void LevelPhysicsMarbles::spawnMarble(const Math::Vec3& pos, float radius, const Color& color) {
    auto mat = Material::standard();
    mat->setColor(color);
    mat->setRoughness(0.15f);
    mat->setMetallic(0.7f);

    auto mesh = std::make_shared<Mesh>(Geometry::sphere(radius, 16, 8), mat);
    mesh->setPosition(pos.x, pos.y, pos.z);
    scene_.add(mesh);

    RigidBodyDesc desc;
    desc.shape = SphereShape{ radius };
    desc.motionType = MotionType::Dynamic;
    desc.position = pos;
    desc.restitution = 0.6f;
    desc.friction = 0.3f;
    desc.mass = radius * radius * radius * 10.0f;
    auto body = physics_.createBody(desc);

    marbles_.push_back({ mesh, body });
}

void LevelPhysicsMarbles::teardown([[maybe_unused]] Engine& engine) {
    for (auto& m : marbles_) {
        physics_.destroyBody(m.body);
    }
    for (auto& w : walls_) {
        physics_.destroyBody(w);
    }
    marbles_.clear();
    walls_.clear();
    physics_.shutdown();
    scene_ = Scene3D{};
    spawned_ = 0;
}

void LevelPhysicsMarbles::update(Engine& engine, float dt) {
    orbitCtrl_.update(engine.input(), dt);

    // Spawn marbles over time
    if (spawned_ < marbleCount_) {
        spawnTimer_ += dt;
        constexpr float spawnInterval = 0.05f;
        while (spawnTimer_ >= spawnInterval && spawned_ < marbleCount_) {
            spawnTimer_ -= spawnInterval;

            static std::mt19937 rng(42);
            std::uniform_real_distribution<float> xzDist(-1.5f, 1.5f);
            std::uniform_real_distribution<float> yDist(5.0f, 8.0f);
            std::uniform_real_distribution<float> radiusDist(0.15f, 0.3f);
            std::uniform_real_distribution<float> colorDist(0.2f, 1.0f);

            float r = radiusDist(rng);
            Color col(colorDist(rng), colorDist(rng), colorDist(rng));
            spawnMarble(Math::Vec3(xzDist(rng), yDist(rng), xzDist(rng)), r, col);
            ++spawned_;
        }
    }

    if (!paused_) {
        physics_.update(dt);
    }

    // Sync mesh transforms from physics
    for (auto& m : marbles_) {
        if (!m.body.isValid()) { continue; }
        Math::Vec3 pos = physics_.getPosition(m.body);
        Math::Quat rot = physics_.getRotation(m.body);

        m.mesh->setPosition(pos.x, pos.y, pos.z);
        Math::Vec3 euler = glm::degrees(glm::eulerAngles(rot));
        m.mesh->setRotation(euler.x, euler.y, euler.z);
    }
}

void LevelPhysicsMarbles::drawUI() {
#ifdef NOX_HAS_IMGUI
    ImGui::Text("Marbles in Container (Jolt)");
    ImGui::Separator();
    ImGui::Checkbox("Paused", &paused_);
    ImGui::Text("Marbles: %d / %d", spawned_, marbleCount_);

    if (ImGui::SliderInt("Target Count", &marbleCount_, 10, 200)) {
        // Increase target — new marbles will spawn on next frames
    }

    if (ImGui::Button("Reset")) {
        // Remove existing marbles
        for (auto& m : marbles_) {
            physics_.destroyBody(m.body);
            scene_.remove(m.mesh);
        }
        marbles_.clear();
        spawned_ = 0;
        spawnTimer_ = 0.0f;
    }
#endif
}

// Copyright (c) 2026 NoxFly — AGPL-3.0

#include "levels/LevelPhysicsBasic.hpp"

#ifdef NOX_HAS_IMGUI
#include <imgui.h>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace Nox;

void LevelPhysicsBasic::setup(Engine& engine) {
    camera_.setAspect(engine.aspect());
    camera_.setPosition(8.0f, 6.0f, 8.0f);
    camera_.lookAt(0.0f, 1.0f, 0.0f);
    orbitCtrl_.syncFromCamera();

    physics_.init();

    // Static ground plane (large flat box)
    {
        auto mat = Material::standard();
        mat->setColor(Color(0.4f, 0.4f, 0.45f));
        mat->setRoughness(0.9f);
        auto mesh = std::make_shared<Mesh>(Geometry::box(20.0f, 0.2f, 20.0f), mat);
        mesh->setPosition(0.0f, -0.1f, 0.0f);
        scene_.add(mesh);

        RigidBodyDesc desc;
        desc.shape = BoxShape{ Math::Vec3(10.0f, 0.1f, 10.0f) };
        desc.motionType = MotionType::Static;
        desc.position = Math::Vec3(0.0f, -0.1f, 0.0f);
        auto body = physics_.createBody(desc);
        objects_.push_back({ mesh, body });
    }

    // Stack of dynamic boxes
    for (int y = 0; y < 5; ++y) {
        for (int x = -1; x <= 1; ++x) {
            spawnBox(Math::Vec3(static_cast<float>(x) * 1.1f,
                                0.5f + static_cast<float>(y) * 1.05f,
                                0.0f));
        }
    }

    // Lights
    auto sun = std::make_shared<DirectionalLight>(Color::White, 0.8f);
    sun->setDirection(-1.0f, -1.0f, -0.5f);
    scene_.add(sun);

    auto ambient = std::make_shared<AmbientLight>(Color::White, 0.1f);
    scene_.add(ambient);
}

void LevelPhysicsBasic::spawnBox(const Math::Vec3& pos) {
    auto mat = Material::standard();
    float r = static_cast<float>(objects_.size() % 7) / 7.0f;
    mat->setColor(Color(0.3f + r * 0.5f, 0.5f, 0.8f - r * 0.4f));
    mat->setRoughness(0.4f);

    auto mesh = std::make_shared<Mesh>(Geometry::box(1.0f, 1.0f, 1.0f), mat);
    mesh->setPosition(pos.x, pos.y, pos.z);
    scene_.add(mesh);

    RigidBodyDesc desc;
    desc.shape = BoxShape{ Math::Vec3(0.5f, 0.5f, 0.5f) };
    desc.motionType = MotionType::Dynamic;
    desc.position = pos;
    desc.restitution = 0.3f;
    auto body = physics_.createBody(desc);

    objects_.push_back({ mesh, body });
}

void LevelPhysicsBasic::teardown([[maybe_unused]] Engine& engine) {
    for (auto& obj : objects_) {
        physics_.destroyBody(obj.body);
    }
    objects_.clear();
    physics_.shutdown();
    scene_ = Scene3D{};
}

void LevelPhysicsBasic::update(Engine& engine, float dt) {
    orbitCtrl_.update(engine.input(), dt);

    if (!paused_) {
        physics_.update(dt);
    }

    // Sync mesh transforms from physics
    for (auto& obj : objects_) {
        if (!obj.body.isValid()) { continue; }
        Math::Vec3 pos = physics_.getPosition(obj.body);
        Math::Quat rot = physics_.getRotation(obj.body);

        obj.mesh->setPosition(pos.x, pos.y, pos.z);
        Math::Vec3 euler = glm::degrees(glm::eulerAngles(rot));
        obj.mesh->setRotation(euler.x, euler.y, euler.z);
    }
}

void LevelPhysicsBasic::drawUI() {
#ifdef NOX_HAS_IMGUI
    ImGui::Text("Basic Physics (Jolt)");
    ImGui::Separator();
    ImGui::Checkbox("Paused", &paused_);
    ImGui::Text("Bodies: %d", static_cast<int>(objects_.size()));

    if (ImGui::Button("Drop Box")) {
        spawnBox(Math::Vec3(0.0f, 8.0f, 0.0f));
    }
#endif
}

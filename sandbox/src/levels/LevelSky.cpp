// Copyright (c) 2026 NoxFly — AGPL-3.0

#include "levels/LevelSky.hpp"

#ifdef NOX_HAS_IMGUI
#include <imgui.h>
#endif

#include <cmath>

using namespace Nox;

void LevelSky::setup(Engine& engine) {
    NOX_LOG_INFO("LevelSky::setup()");
    engine_ = &engine;

    camera_.setAspect(engine.aspect());
    camera_.setPosition(8.0f, 4.0f, 8.0f);
    camera_.lookAt(0.0f, 0.0f, 0.0f);
    orbitCtrl_.syncFromCamera();

    // Initialize sky
    sky_.init();
    sky_.setTurbidity(3.0f);
    sky_.setSunIntensity(22.0f);

    // Initialize text renderer
    textRenderer_.init();

    // Floor
    auto floorMat = Material::standard();
    floorMat->setColor(Color(0.3f, 0.5f, 0.3f));
    floorMat->setRoughness(0.9f);
    auto floor = std::make_shared<Mesh>(Geometry::plane(40.0f, 40.0f), floorMat);
    floor->setName("Ground");
    scene_.add(floor);

    // Some objects to pick
    for (int i = 0; i < 5; ++i) {
        auto mat = Material::standard();
        float hue = static_cast<float>(i) / 5.0f;
        mat->setColor(Color(
            0.5f + 0.5f * std::cos(hue * 6.28f),
            0.5f + 0.5f * std::cos(hue * 6.28f + 2.09f),
            0.5f + 0.5f * std::cos(hue * 6.28f + 4.19f)
        ));
        mat->setRoughness(0.3f);
        mat->setMetallic(0.5f);

        auto mesh = std::make_shared<Mesh>(Geometry::sphere(0.5f, 32, 16), mat);
        mesh->setName("Sphere_" + std::to_string(i));
        mesh->setPosition(
            static_cast<float>(i - 2) * 2.5f,
            0.5f,
            0.0f
        );
        scene_.add(mesh);
    }

    // Add some boxes too
    for (int i = 0; i < 3; ++i) {
        auto mat = Material::standard();
        mat->setColor(Color(0.9f, 0.7f, 0.3f));
        mat->setRoughness(0.4f);

        auto mesh = std::make_shared<Mesh>(Geometry::box(0.8f, 0.8f, 0.8f), mat);
        mesh->setName("Box_" + std::to_string(i));
        mesh->setPosition(
            static_cast<float>(i - 1) * 3.0f,
            0.4f,
            3.0f
        );
        scene_.add(mesh);
    }

    // Lights (minimal, sky provides most lighting)
    auto sun = std::make_shared<DirectionalLight>(Color::White, 1.0f);
    sun->setName("Sun");
    scene_.add(sun);

    auto ambient = std::make_shared<AmbientLight>(Color::White, 0.15f);
    ambient->setName("Ambient");
    scene_.add(ambient);

    NOX_LOG_INFO("LevelSky::setup() done");
}

void LevelSky::teardown([[maybe_unused]] Engine& engine) {
    scene_ = Scene3D{};
    selectedMesh_.reset();
    engine_ = nullptr;
}

void LevelSky::update(Engine& engine, float dt) {
    orbitCtrl_.update(engine.input(), dt);

    // Update sun direction based on time of day
    float sunAngle = timeOfDay_ * glm::pi<float>();
    Math::Vec3 sunDir(
        std::cos(sunAngle) * 0.5f,
        std::sin(sunAngle),
        -0.3f
    );
    sky_.setSunDirection(sunDir);

    // Update the directional light to match the sun
    auto dirLights = scene_.findAllOfType<DirectionalLight>();
    if (!dirLights.empty()) {
        dirLights[0]->setDirection(-sunDir);
        // Adjust light color based on sun height
        float sunHeight = std::max(sunDir.y, 0.0f);
        float warmth = 1.0f - sunHeight;
        dirLights[0]->setColor(Color(1.0f, 1.0f - warmth * 0.3f, 1.0f - warmth * 0.5f));
        dirLights[0]->setIntensity(sunHeight * 1.5f);
    }

    // Render sky BEFORE main scene render
    // The sky renders into the current framebuffer (HDR)
    // This is done via drawUI since it's called during the frame
    sky_.render(camera_.viewMatrix(), camera_.projectionMatrix(), camera_.position());

    // Object picking on left click
    auto& input = engine.input();
    if (input.isMouseButtonPressed(MouseButton::Left)) {
        auto [w, h] = engine.window().size();
        auto picked = picker_.pickFromMouse(scene_, camera_, input, w, h);
        if (picked) {
            selectedMesh_ = picked;
            NOX_LOG_INFO("Picked: {}", picked->name());
        }
        else {
            selectedMesh_.reset();
        }
    }
}

void LevelSky::drawUI() {
#ifdef NOX_HAS_IMGUI
    ImGui::Text("Sky & Object Picking");
    ImGui::Separator();

    // Time of day
    ImGui::SliderFloat("Time of Day", &timeOfDay_, 0.0f, 1.0f, "%.2f");

    // Sky parameters
    if (ImGui::CollapsingHeader("Sky Settings")) {
        float turbidity = sky_.turbidity();
        if (ImGui::SliderFloat("Turbidity", &turbidity, 1.0f, 20.0f)) {
            sky_.setTurbidity(turbidity);
        }

        float intensity = sky_.sunIntensity();
        if (ImGui::DragFloat("Sun Intensity", &intensity, 0.1f, 0.0f, 100.0f)) {
            sky_.setSunIntensity(intensity);
        }

        float rayleigh = sky_.rayleighCoefficient();
        if (ImGui::DragFloat("Rayleigh", &rayleigh, 0.1f, 0.0f, 10.0f)) {
            sky_.setRayleighCoefficient(rayleigh);
        }

        float mie = sky_.mieCoefficient();
        if (ImGui::DragFloat("Mie", &mie, 0.0001f, 0.0f, 0.1f, "%.4f")) {
            sky_.setMieCoefficient(mie);
        }

        float mieG = sky_.mieDirectionalG();
        if (ImGui::SliderFloat("Mie G", &mieG, -0.99f, 0.99f)) {
            sky_.setMieDirectionalG(mieG);
        }
    }

    ImGui::Separator();
    ImGui::Text("Click on objects to select them");
    if (selectedMesh_) {
        ImGui::Text("Selected: %s", selectedMesh_->name().c_str());
    }
    else {
        ImGui::TextDisabled("Nothing selected");
    }
#endif

    // Render text overlay
    if (engine_ && textRenderer_.isInitialized()) {
        auto [w, h] = engine_->window().size();
        textRenderer_.drawText("NoxEngine v1.0", 10.0f, static_cast<float>(h) - 30.0f,
                               w, h, 2.0f, Math::Vec3(1.0f, 1.0f, 1.0f));
    }
}

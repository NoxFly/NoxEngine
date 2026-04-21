// Copyright (c) 2026 NoxFly — AGPL-3.0

#include "levels/LevelEditor.hpp"

#include <NoxEngine/editor/SceneSerializer.hpp>

#ifdef NOX_HAS_IMGUI
#include <imgui.h>
#endif

using namespace Nox;

void LevelEditor::setup(Engine& engine) {
    NOX_LOG_INFO("LevelEditor::setup()");
    engine_ = &engine;

    camera_.setAspect(engine.aspect());
    camera_.setPosition(5.0f, 3.0f, 5.0f);
    camera_.lookAt(0.0f, 0.5f, 0.0f);
    orbitCtrl_.syncFromCamera();

    // Create a simple default scene for the editor
    auto floorMat = Material::standard();
    floorMat->setColor(Color(0.4f, 0.4f, 0.45f));
    floorMat->setRoughness(0.9f);
    auto floor = std::make_shared<Mesh>(Geometry::plane(20.0f, 20.0f), floorMat);
    floor->setName("Floor");
    scene_.add(floor);

    auto boxMat = Material::standard();
    boxMat->setColor(Color(0.8f, 0.4f, 0.2f));
    boxMat->setRoughness(0.3f);
    auto box = std::make_shared<Mesh>(Geometry::box(1.0f, 1.0f, 1.0f), boxMat);
    box->setName("Box");
    box->setPosition(0.0f, 0.5f, 0.0f);
    scene_.add(box);

    auto sphereMat = Material::standard();
    sphereMat->setColor(Color(0.2f, 0.6f, 0.9f));
    sphereMat->setMetallic(0.8f);
    sphereMat->setRoughness(0.1f);
    auto sphere = std::make_shared<Mesh>(Geometry::sphere(0.5f, 32, 16), sphereMat);
    sphere->setName("Sphere");
    sphere->setPosition(2.0f, 0.5f, 0.0f);
    scene_.add(sphere);

    auto cylMat = Material::standard();
    cylMat->setColor(Color(0.3f, 0.8f, 0.3f));
    cylMat->setRoughness(0.5f);
    auto cyl = std::make_shared<Mesh>(Geometry::cylinder(0.4f, 0.4f, 1.0f, 24), cylMat);
    cyl->setName("Cylinder");
    cyl->setPosition(-2.0f, 0.5f, 0.0f);
    scene_.add(cyl);

    auto sun = std::make_shared<DirectionalLight>(Color::White, 0.8f);
    sun->setName("Sun");
    sun->setDirection(-1.0f, -1.0f, -0.5f);
    scene_.add(sun);

    auto ambient = std::make_shared<AmbientLight>(Color::White, 0.1f);
    ambient->setName("Ambient");
    scene_.add(ambient);

    auto point = std::make_shared<PointLight>(Color(1.0f, 0.8f, 0.6f), 2.0f);
    point->setName("PointLight");
    point->setPosition(2.0f, 3.0f, 2.0f);
    point->setRange(12.0f);
    scene_.add(point);

    // Set initial gizmo target
    gizmo_.setTarget(box.get());
    hierarchy_.setSelected(box.get());

    assetBrowser_.setRootDirectory(".");

    NOX_LOG_INFO("LevelEditor::setup() done");
}

void LevelEditor::teardown([[maybe_unused]] Engine& engine) {
    scene_ = Scene3D{};
    gizmo_.setTarget(nullptr);
    hierarchy_.clearSelection();
    engine_ = nullptr;
}

void LevelEditor::update(Engine& engine, float dt) {
    auto& input = engine.input();

    // Gizmo mode shortcuts
    if (input.isKeyPressed(Key::W)) { gizmo_.setMode(Gizmo::Mode::Translate); }
    if (input.isKeyPressed(Key::E)) { gizmo_.setMode(Gizmo::Mode::Rotate); }
    if (input.isKeyPressed(Key::R)) { gizmo_.setMode(Gizmo::Mode::Scale); }

    // Update gizmo (if it consumes input, skip camera update)
    auto [w, h] = engine.window().size();
    bool gizmoActive = gizmo_.update(input, camera_, w, h);

    if (!gizmoActive) {
        orbitCtrl_.update(input, dt);
    }

    // Sync gizmo target with hierarchy selection
    auto* selected = hierarchy_.selected();
    if (gizmo_.target() != selected) {
        gizmo_.setTarget(selected);
    }
}

void LevelEditor::drawUI() {
#ifdef NOX_HAS_IMGUI
    // Scene hierarchy panel
    hierarchy_.draw(scene_);

    // Property inspector for selected object
    inspector_.draw(hierarchy_.selected());

    // Asset browser
    assetBrowser_.draw();

    // Gizmo controls
    ImGui::SetNextWindowPos(ImVec2(10, 610), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(250, 120), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Gizmo")) {
        int mode = static_cast<int>(gizmo_.mode());
        ImGui::RadioButton("Translate (W)", &mode, 0); ImGui::SameLine();
        ImGui::RadioButton("Rotate (E)", &mode, 1); ImGui::SameLine();
        ImGui::RadioButton("Scale (R)", &mode, 2);
        gizmo_.setMode(static_cast<Nox::Gizmo::Mode>(mode));

        ImGui::Separator();

        // Save / Load
        if (ImGui::Button("Save Scene")) {
            [[maybe_unused]] bool saved = SceneSerializer::save(scene_, "scene.nox");
        }
        ImGui::SameLine();
        if (ImGui::Button("Load Scene")) {
            scene_ = Scene3D{};
            [[maybe_unused]] bool loaded = SceneSerializer::load(scene_, "scene.nox");
            hierarchy_.clearSelection();
            gizmo_.setTarget(nullptr);
        }
    }
    ImGui::End();

    // Render gizmo overlay (after scene render, before ImGui submits)
    if (engine_) {
        auto [w, h] = engine_->window().size();
        gizmo_.render(camera_.viewMatrix(), camera_.projectionMatrix(), w, h);
    }
#endif
}

// Copyright (c) 2026 NoxFly — AGPL-3.0

#include "levels/LevelAdvancedRendering.hpp"

#ifdef NOX_HAS_IMGUI
#include <imgui.h>
#endif

#include <cmath>

using namespace Nox;

void LevelAdvancedRendering::setup(Engine& engine) {
    NOX_LOG_INFO("LevelAdvancedRendering::setup()");
    engine_ = &engine;

    camera_.setAspect(engine.aspect());
    camera_.setPosition(10.0f, 8.0f, 10.0f);
    camera_.lookAt(0.0f, 0.0f, 0.0f);
    orbitCtrl_.syncFromCamera();

    auto [w, h] = engine.window().size();

    // Initialize G-Buffer
    gBuffer_.init(w, h);

    // Initialize CSM
    CascadedShadowMap::Config csmConfig;
    csmConfig.numCascades = 4;
    csmConfig.shadowMapSize = 2048;
    csmConfig.maxShadowDistance = 200.0f;
    csm_.init(csmConfig);

    // Initialize clustered lighting
    ClusteredLighting::Config clConfig;
    clConfig.gridSizeX = 16;
    clConfig.gridSizeY = 9;
    clConfig.gridSizeZ = 24;
    clusteredLighting_.init(clConfig);

    // Initialize decal renderer
    decalRenderer_.init();

    // Add SSR, TAA, and Volumetric Fog to post-process stack
    auto ssr = std::make_unique<SSREffect>();
    ssr->init(w, h);
    ssrEffect_ = ssr.get();
    engine.postProcessStack().addEffect(std::move(ssr));

    auto taa = std::make_unique<TAAEffect>();
    taa->init(w, h);
    taaEffect_ = taa.get();
    engine.postProcessStack().addEffect(std::move(taa));

    auto fog = std::make_unique<VolumetricFogEffect>();
    fog->init(w, h);
    fogEffect_ = fog.get();
    engine.postProcessStack().addEffect(std::move(fog));

    // ── Scene setup ───────────────────────────────────────────

    // Large ground plane
    auto groundMat = Material::standard();
    groundMat->setColor(Color(0.4f, 0.4f, 0.4f));
    groundMat->setRoughness(0.8f);
    auto ground = std::make_shared<Mesh>(Geometry::plane(50.0f, 50.0f), groundMat);
    ground->setName("Ground");
    scene_.add(ground);

    // Reflective floor patch for SSR demo
    auto reflectiveMat = Material::standard();
    reflectiveMat->setColor(Color(0.8f, 0.8f, 0.9f));
    reflectiveMat->setRoughness(0.05f);
    reflectiveMat->setMetallic(0.9f);
    auto reflFloor = std::make_shared<Mesh>(Geometry::plane(10.0f, 10.0f), reflectiveMat);
    reflFloor->setPosition(0.0f, 0.01f, 0.0f);
    reflFloor->setName("Reflective Floor");
    scene_.add(reflFloor);

    // Central column
    auto colMat = Material::standard();
    colMat->setColor(Color(0.9f, 0.9f, 0.9f));
    colMat->setRoughness(0.3f);
    auto column = std::make_shared<Mesh>(Geometry::cylinder(0.5f, 0.5f, 4.0f, 32), colMat);
    column->setPosition(0.0f, 2.0f, 0.0f);
    column->setName("Column");
    scene_.add(column);

    // Sphere on top
    auto sphereMat = Material::standard();
    sphereMat->setColor(Color(1.0f, 0.3f, 0.3f));
    sphereMat->setRoughness(0.1f);
    sphereMat->setMetallic(0.8f);
    auto sphere = std::make_shared<Mesh>(Geometry::sphere(0.8f, 32, 16), sphereMat);
    sphere->setPosition(0.0f, 4.8f, 0.0f);
    sphere->setName("Metallic Sphere");
    scene_.add(sphere);

    // Many point lights for clustered lighting demo
    for (int i = 0; i < 32; ++i) {
        float angle = static_cast<float>(i) / 32.0f * glm::two_pi<float>();
        float radius = 8.0f + static_cast<float>(i % 3) * 3.0f;
        float height = 1.0f + static_cast<float>(i % 4) * 0.5f;

        Color lightColor(
            0.5f + 0.5f * std::cos(angle),
            0.5f + 0.5f * std::cos(angle + 2.09f),
            0.5f + 0.5f * std::cos(angle + 4.19f)
        );

        auto pl = std::make_shared<PointLight>(lightColor, 1.5f);
        pl->setPosition(std::cos(angle) * radius, height, std::sin(angle) * radius);
        pl->setRange(6.0f);
        pl->setName("PointLight_" + std::to_string(i));
        scene_.add(pl);
    }

    // Directional light (main sun for CSM)
    auto sun = std::make_shared<DirectionalLight>(Color::White, 1.2f);
    sun->setDirection(-0.5f, -1.0f, -0.3f);
    sun->setName("Sun");
    scene_.add(sun);

    auto ambient = std::make_shared<AmbientLight>(Color::White, 0.1f);
    ambient->setName("Ambient");
    scene_.add(ambient);

    // A few decals on the ground
    for (int i = 0; i < 5; ++i) {
        auto decal = std::make_shared<Decal>(1.5f, 0.5f, 1.5f);
        decal->setPosition(
            static_cast<float>(i - 2) * 3.0f,
            0.1f,
            -3.0f
        );
        decal->setName("Decal_" + std::to_string(i));
        decal->setOpacity(0.8f);
        decals_.push_back(decal);
        scene_.add(decal);
    }

    NOX_LOG_INFO("LevelAdvancedRendering::setup() done — {} lights, {} decals",
                 32, decals_.size());
}

void LevelAdvancedRendering::teardown([[maybe_unused]] Engine& engine) {
    // Remove effects from post-process stack
    engine.postProcessStack().removeEffect("SSR");
    engine.postProcessStack().removeEffect("TAA");
    engine.postProcessStack().removeEffect("Volumetric Fog");

    scene_ = Scene3D{};
    decals_.clear();
    ssrEffect_ = nullptr;
    taaEffect_ = nullptr;
    fogEffect_ = nullptr;
    engine_ = nullptr;
}

void LevelAdvancedRendering::update(Engine& engine, float dt) {
    orbitCtrl_.update(engine.input(), dt);

    // Update CSM with current camera and sun direction
    auto dirLights = scene_.findAllOfType<DirectionalLight>();
    if (!dirLights.empty() && enableCSM_) {
        csm_.update(camera_.viewMatrix(), camera_.projectionMatrix(),
                    camera_.near(), camera_.far(),
                    dirLights[0]->direction());
    }

    // Update clustered lighting
    if (enableClustered_) {
        auto pointLights = scene_.findAllOfType<PointLight>();
        auto [w, h] = engine.window().size();
        clusteredLighting_.update(camera_.viewMatrix(), camera_.projectionMatrix(),
                                  camera_.near(), camera_.far(),
                                  pointLights, w, h);
    }

    // Update TAA jitter
    if (taaEffect_ && enableTAA_) {
        taaEffect_->nextFrame();
    }

    // Update effect enabled states
    if (ssrEffect_) { ssrEffect_->setEnabled(enableSSR_); }
    if (taaEffect_) { taaEffect_->setEnabled(enableTAA_); }
    if (fogEffect_) { fogEffect_->setEnabled(enableFog_); }
}

void LevelAdvancedRendering::drawUI() {
#ifdef NOX_HAS_IMGUI
    ImGui::Text("Advanced Rendering (v1.1)");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("G-Buffer", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Show G-Buffer", &showGBuffer_);
        if (showGBuffer_) {
            const char* displays[] = { "Final", "Position", "Normal", "Albedo", "Emissive" };
            ImGui::Combo("Display", &gBufferDisplay_, displays, 5);
        }
    }

    if (ImGui::CollapsingHeader("Shadows")) {
        ImGui::Checkbox("Cascaded Shadow Maps", &enableCSM_);
        if (enableCSM_) {
            int numCascades = csm_.numCascades();
            ImGui::Text("Cascades: %d", numCascades);
            for (int i = 0; i < numCascades + 1; ++i) {
                ImGui::Text("  Split %d: %.1f", i, csm_.cascadeSplits()[i]);
            }
        }
    }

    if (ImGui::CollapsingHeader("Lighting")) {
        ImGui::Checkbox("Clustered Lighting", &enableClustered_);
        if (enableClustered_) {
            ImGui::Text("Grid: %dx%dx%d = %d clusters",
                        clusteredLighting_.config().gridSizeX,
                        clusteredLighting_.config().gridSizeY,
                        clusteredLighting_.config().gridSizeZ,
                        clusteredLighting_.totalClusters());
        }
    }

    if (ImGui::CollapsingHeader("Post Effects")) {
        ImGui::Checkbox("SSR", &enableSSR_);
        if (enableSSR_ && ssrEffect_) {
            int steps = ssrEffect_->maxSteps();
            if (ImGui::SliderInt("SSR Steps", &steps, 8, 256)) {
                ssrEffect_->setMaxSteps(steps);
            }
            float thickness = ssrEffect_->thickness();
            if (ImGui::DragFloat("SSR Thickness", &thickness, 0.01f, 0.01f, 5.0f)) {
                ssrEffect_->setThickness(thickness);
            }
        }

        ImGui::Checkbox("TAA", &enableTAA_);
        if (enableTAA_ && taaEffect_) {
            float blend = taaEffect_->blendFactor();
            if (ImGui::SliderFloat("TAA Blend", &blend, 0.0f, 1.0f)) {
                taaEffect_->setBlendFactor(blend);
            }
        }

        ImGui::Checkbox("Volumetric Fog", &enableFog_);
        if (enableFog_ && fogEffect_) {
            float density = fogEffect_->density();
            if (ImGui::DragFloat("Fog Density", &density, 0.001f, 0.0f, 0.5f, "%.3f")) {
                fogEffect_->setDensity(density);
            }
            float scattering = fogEffect_->scattering();
            if (ImGui::SliderFloat("Fog Scattering", &scattering, 0.0f, 1.0f)) {
                fogEffect_->setScattering(scattering);
            }
            int steps = fogEffect_->numSteps();
            if (ImGui::SliderInt("Fog Steps", &steps, 8, 128)) {
                fogEffect_->setNumSteps(steps);
            }
        }
    }

    if (ImGui::CollapsingHeader("Decals")) {
        ImGui::Checkbox("Enable Decals", &enableDecals_);
        ImGui::Text("Active Decals: %d", static_cast<int>(decals_.size()));
    }
#endif
}

// Copyright (c) 2026 NoxFly — AGPL-3.0

#include "LevelTerrain.hpp"

#include <GL/glew.h>

#ifdef NOX_HAS_IMGUI
#include <imgui.h>
#endif

void LevelTerrain::setup(Nox::Engine& engine) {
    auto [w, h] = engine.window().size();
    camera_ = Nox::PerspectiveCamera(45.0f, static_cast<float>(w) / static_cast<float>(h),
                                      0.1f, 2000.0f);
    camera_.setPosition(32.0f, 30.0f, 32.0f);
    camera_.lookAt(128.0f, 0.0f, 128.0f);
    orbitController_.syncFromCamera();

    // ── Terrain setup ─────────────────────────────────────────────
    Nox::Terrain::Config terrainCfg;
    terrainCfg.chunkSize    = 33;   // 32+1 for seamless stitching
    terrainCfg.numChunksX   = 8;
    terrainCfg.numChunksZ   = 8;
    terrainCfg.worldScale   = 1.0f;
    terrainCfg.heightScale  = 50.0f;
    terrainCfg.maxLodLevels = 4;
    terrainCfg.lodDistanceFactor = 2.5f;

    terrain_.init(terrainCfg);
    terrain_.generateFromNoise(terrainSeed_, 0.008f, 6);

    // ── Lighting ──────────────────────────────────────────────────
    auto sun = std::make_shared<Nox::DirectionalLight>(Nox::Color(1.0f, 0.95f, 0.85f), 1.2f);
    sun->setDirection(-0.4f, -0.8f, -0.3f);
    scene_.add(sun);

    auto ambient = std::make_shared<Nox::AmbientLight>(Nox::Color::White, 0.15f);
    scene_.add(ambient);

    // ── Marker meshes on terrain ──────────────────────────────────
    auto mat = Nox::Material::standard();
    mat->setColor(Nox::Color(0.8f, 0.2f, 0.2f));
    mat->setRoughness(0.5f);

    auto sphereGeo = Nox::Geometry::sphere(1.0f, 16, 12);

    for (int i = 0; i < 10; ++i) {
        float x = 30.0f + static_cast<float>(i) * 20.0f;
        float z = 128.0f;
        float y = terrain_.heightAt(x, z) + 1.0f;

        auto marker = std::make_shared<Nox::Mesh>(sphereGeo, mat);
        marker->setPosition(x, y, z);
        scene_.add(marker);
    }

    // ── Occlusion culling ─────────────────────────────────────────
    occlusionCulling_.init(w, h);
    occlusionCulling_.setEnabled(enableOcclusion_);

    // ── Imposter system ───────────────────────────────────────────
    Nox::ImposterSystem::Config impCfg;
    impCfg.atlasSize     = 1024;
    impCfg.viewsPerRow   = 8;
    impCfg.viewRows      = 4;
    impCfg.switchDistance = 80.0f;
    imposters_.init(impCfg);
}

void LevelTerrain::teardown([[maybe_unused]] Nox::Engine& engine) {
    scene_ = Nox::Scene3D();
}

void LevelTerrain::update(Nox::Engine& engine, float dt) {
    auto& input = engine.input();
    orbitController_.update(input, dt);

    Nox::Math::Vec3 camPos = camera_.position();

    // ── Update systems ────────────────────────────────────────────
    terrain_.update(camPos);
    occlusionCulling_.setEnabled(enableOcclusion_);

    // ── Render terrain ────────────────────────────────────────────
    auto viewMat = camera_.viewMatrix();
    auto projMat = camera_.projectionMatrix();

    if (showWireframe_) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
    terrain_.render(viewMat, projMat, camPos);
    if (showWireframe_) { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

    engine.render(scene_, camera_);
}

void LevelTerrain::drawUI() {
#ifdef NOX_HAS_IMGUI
    ImGui::Begin("Terrain Controls");

    ImGui::SeparatorText("Terrain");
    if (ImGui::SliderInt("Seed", &terrainSeed_, 0, 9999)) {
        terrain_.generateFromNoise(terrainSeed_, 0.008f, 6);
    }
    ImGui::Checkbox("Wireframe", &showWireframe_);

    ImGui::SeparatorText("Camera");

    Nox::Math::Vec3 pos = camera_.position();
    float terrainH = terrain_.heightAt(pos.x, pos.z);
    ImGui::Text("Camera: (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
    ImGui::Text("Terrain height: %.1f", terrainH);

    ImGui::SeparatorText("Occlusion Culling");
    ImGui::Checkbox("Enabled##occlusion", &enableOcclusion_);
    if (occlusionCulling_.isInitialized()) {
        ImGui::Text("HZB levels: %d", occlusionCulling_.hzbLevels());
    }

    ImGui::SeparatorText("Vegetation");
    ImGui::Checkbox("Show##vegetation", &enableVegetation_);

    ImGui::End();
#endif
}

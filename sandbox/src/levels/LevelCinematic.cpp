// Copyright (c) 2026 NoxFly — AGPL-3.0

#include "levels/LevelCinematic.hpp"

#ifdef NOX_HAS_IMGUI
#include <imgui.h>
#endif

using namespace Nox;

void LevelCinematic::setup(Engine& engine) {
    engine_ = &engine;
    camera_.setAspect(engine.aspect());
    camera_.setPosition(6.0f, 4.0f, 6.0f);
    camera_.lookAt(0.0f, 0.5f, 0.0f);
    orbitCtrl_.syncFromCamera();

    // ── Scene setup ───────────────────────────────────────────────

    // Ground
    auto floorMat = Material::standard();
    floorMat->setColor(Color(0.15f, 0.15f, 0.18f));
    floorMat->setRoughness(0.9f);
    auto floor = std::make_shared<Mesh>(Geometry::plane(30.0f, 30.0f), floorMat);
    scene_.add(floor);

    // Central pedestal with emissive glow (bloom interaction)
    auto pedestalMat = Material::standard();
    pedestalMat->setColor(Color(0.3f, 0.3f, 0.35f));
    pedestalMat->setRoughness(0.5f);
    auto pedestal = std::make_shared<Mesh>(Geometry::box(1.5f, 0.3f, 1.5f), pedestalMat);
    pedestal->setPosition(0.0f, 0.15f, 0.0f);
    scene_.add(pedestal);

    // Glowing orb on top of pedestal
    auto orbMat = Material::standard();
    orbMat->setColor(Color(0.02f, 0.02f, 0.02f));
    orbMat->setEmissiveColor(Color(0.4f, 0.7f, 1.0f));
    orbMat->setEmissiveIntensity(8.0f);
    orbMat->setRoughness(0.1f);
    orbMat->setMetallic(0.9f);
    auto orb = std::make_shared<Mesh>(Geometry::sphere(0.6f, 32, 16), orbMat);
    orb->setPosition(0.0f, 1.0f, 0.0f);
    scene_.add(orb);

    // Columns around the scene (good for vignette + depth)
    auto colMat = Material::standard();
    colMat->setColor(Color(0.5f, 0.45f, 0.4f));
    colMat->setRoughness(0.7f);

    constexpr int NumColumns = 8;
    constexpr float Radius   = 6.0f;
    for (int i = 0; i < NumColumns; ++i) {
        float angle = static_cast<float>(i) * 6.2832f / static_cast<float>(NumColumns);
        float x = Radius * std::cos(angle);
        float z = Radius * std::sin(angle);
        auto col = std::make_shared<Mesh>(Geometry::box(0.4f, 3.0f, 0.4f), colMat);
        col->setPosition(x, 1.5f, z);
        scene_.add(col);
    }

    // ── Lights ────────────────────────────────────────────────────
    auto sun = std::make_shared<DirectionalLight>(Color(1.0f, 0.92f, 0.8f), 0.6f);
    sun->setDirection(-0.5f, -1.0f, -0.3f);
    scene_.add(sun);

    auto ambient = std::make_shared<AmbientLight>(Color::White, 0.06f);
    scene_.add(ambient);

    auto warm = std::make_shared<PointLight>(Color(1.0f, 0.6f, 0.3f), 4.0f);
    warm->setPosition(0.0f, 2.5f, 0.0f);
    warm->setRange(12.0f);
    scene_.add(warm);

    // ── Register cinematic effects into the post-process stack ────
    auto& stack = engine.postProcessStack();

    stack.addEffect(std::make_unique<FilmGrainEffect>());
    stack.addEffect(std::make_unique<VignetteEffect>());
    stack.addEffect(std::make_unique<ChromaticAberrationEffect>());

    // Enable bloom for glow interaction
    if (auto* bloom = stack.getEffect("Bloom")) {
        bloom->setEnabled(true);
        bloomEnabled_ = true;
    }

    engine.setExposure(exposure_);
}

void LevelCinematic::teardown(Engine& engine) {
    scene_ = Scene3D{};

    // Remove the cinematic effects we added
    // (They remain in the stack but disable them so they don't affect other levels)
    auto& stack = engine.postProcessStack();
    if (auto* e = stack.getEffect("Film Grain"))           { e->setEnabled(false); }
    if (auto* e = stack.getEffect("Vignette"))             { e->setEnabled(false); }
    if (auto* e = stack.getEffect("Chromatic Aberration")) { e->setEnabled(false); }
    if (auto* e = stack.getEffect("Bloom"))                { e->setEnabled(false); }

    engine_ = nullptr;
}

void LevelCinematic::update(Engine& engine, float dt) {
    orbitCtrl_.update(engine.input(), dt);
}

void LevelCinematic::drawUI() {
#ifdef NOX_HAS_IMGUI
    if (!engine_) { return; }

    ImGui::Begin("Cinematic FX");

    auto& stack = engine_->postProcessStack();

    // Exposure
    if (ImGui::SliderFloat("Exposure", &exposure_, 0.1f, 5.0f)) {
        engine_->setExposure(exposure_);
    }

    ImGui::Separator();

    // Bloom (already in stack from engine init)
    if (auto* bloom = stack.getEffect("Bloom")) {
        if (ImGui::Checkbox("Bloom", &bloomEnabled_)) {
            bloom->setEnabled(bloomEnabled_);
        }
    }

    ImGui::Separator();

    // Film Grain
    if (auto* grain = stack.getEffect("Film Grain")) {
        if (ImGui::Checkbox("Film Grain", &grainEnabled_)) {
            grain->setEnabled(grainEnabled_);
        }
        if (grainEnabled_) {
            auto* g = static_cast<Nox::FilmGrainEffect*>(grain);
            if (ImGui::SliderFloat("Grain Intensity", &grainIntensity_, 0.0f, 0.3f)) {
                g->setIntensity(grainIntensity_);
            }
        }
    }

    // Vignette
    if (auto* vignette = stack.getEffect("Vignette")) {
        if (ImGui::Checkbox("Vignette", &vignetteEnabled_)) {
            vignette->setEnabled(vignetteEnabled_);
        }
        if (vignetteEnabled_) {
            auto* v = static_cast<Nox::VignetteEffect*>(vignette);
            if (ImGui::SliderFloat("Vignette Intensity", &vignetteIntensity_, 0.0f, 2.0f)) {
                v->setIntensity(vignetteIntensity_);
            }
            if (ImGui::SliderFloat("Vignette Smoothness", &vignetteSmoothness_, 0.01f, 1.0f)) {
                v->setSmoothness(vignetteSmoothness_);
            }
        }
    }

    // Chromatic Aberration
    if (auto* chroma = stack.getEffect("Chromatic Aberration")) {
        if (ImGui::Checkbox("Chromatic Aberration", &chromaEnabled_)) {
            chroma->setEnabled(chromaEnabled_);
        }
        if (chromaEnabled_) {
            auto* ca = static_cast<Nox::ChromaticAberrationEffect*>(chroma);
            if (ImGui::SliderFloat("CA Intensity", &chromaIntensity_, 0.0f, 0.02f)) {
                ca->setIntensity(chromaIntensity_);
            }
        }
    }

    ImGui::End();
#endif
}

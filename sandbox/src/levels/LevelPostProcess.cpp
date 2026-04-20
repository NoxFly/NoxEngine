// Copyright (c) 2026 NoxFly — AGPL-3.0

#include "levels/LevelPostProcess.hpp"

#ifdef NOX_HAS_IMGUI
#include <imgui.h>
#endif

using namespace Nox;

void LevelPostProcess::setup(Engine& engine) {
    engine_ = &engine;
    camera_.setAspect(engine.aspect());
    camera_.setPosition(5.0f, 3.0f, 5.0f);
    camera_.lookAt(0.0f, 0.0f, 0.0f);
    orbitCtrl_.syncFromCamera();

    // Floor
    auto floorMat = Material::standard();
    floorMat->setColor(Color(0.4f, 0.4f, 0.45f));
    floorMat->setRoughness(0.8f);
    auto floor = std::make_shared<Mesh>(Geometry::plane(20.0f, 20.0f), floorMat);
    scene_.add(floor);

    // Bright emissive objects (for bloom testing)
    for (int i = 0; i < 5; ++i) {
        auto mat = Material::standard();
        float hue = static_cast<float>(i) / 5.0f;
        mat->setColor(Color(0.05f, 0.05f, 0.05f));
        mat->setEmissiveColor(Color(
            0.5f + 0.5f * std::cos(hue * 6.28f),
            0.5f + 0.5f * std::cos(hue * 6.28f + 2.09f),
            0.5f + 0.5f * std::cos(hue * 6.28f + 4.19f)
        ));
        mat->setEmissiveIntensity(5.0f);
        auto mesh = std::make_shared<Mesh>(Geometry::sphere(0.4f, 24, 12), mat);
        mesh->setPosition(-4.0f + static_cast<float>(i) * 2.0f, 1.0f, 0.0f);
        scene_.add(mesh);
    }

    // Dense geometry for SSAO testing (pillars, boxes close together)
    for (int x = -2; x <= 2; ++x) {
        for (int z = -2; z <= 2; ++z) {
            auto mat = Material::standard();
            mat->setColor(Color(0.6f, 0.6f, 0.65f));
            mat->setRoughness(0.7f);
            float h = 0.5f + static_cast<float>((x + z + 4) % 3) * 0.5f;
            auto mesh = std::make_shared<Mesh>(Geometry::box(0.6f, h, 0.6f), mat);
            mesh->setPosition(static_cast<float>(x) * 1.5f, h * 0.5f,
                              static_cast<float>(z) * 1.5f - 5.0f);
            scene_.add(mesh);
        }
    }

    // Lights
    auto sun = std::make_shared<DirectionalLight>(Color::White, 0.7f);
    sun->setDirection(-1.0f, -1.0f, -0.5f);
    scene_.add(sun);

    auto ambient = std::make_shared<AmbientLight>(Color::White, 0.1f);
    scene_.add(ambient);

    auto point = std::make_shared<PointLight>(Color(1.0f, 0.9f, 0.7f), 3.0f);
    point->setPosition(0.0f, 3.0f, 0.0f);
    point->setRange(15.0f);
    scene_.add(point);

    // Sync UI state with engine
    auto* fxaa = engine.postProcessStack().getEffect("FXAA");
    if (fxaa) { fxaaEnabled_ = fxaa->isEnabled(); }
    auto* bloom = engine.postProcessStack().getEffect("Bloom");
    if (bloom) { bloomEnabled_ = bloom->isEnabled(); }
    auto* ssao = engine.postProcessStack().getEffect("SSAO");
    if (ssao) { ssaoEnabled_ = ssao->isEnabled(); }
    exposure_ = engine.exposure();
}

void LevelPostProcess::teardown([[maybe_unused]] Engine& engine) {
    scene_ = Scene3D{};
    engine_ = nullptr;
}

void LevelPostProcess::update(Engine& engine, float dt) {
    orbitCtrl_.update(engine.input(), dt);
}

void LevelPostProcess::drawUI() {
#ifdef NOX_HAS_IMGUI
    if (!engine_) { return; }

    ImGui::Text("Post-Processing Controls");
    ImGui::Separator();

    // Exposure
    if (ImGui::SliderFloat("Exposure", &exposure_, 0.1f, 5.0f)) {
        engine_->setExposure(exposure_);
    }

    ImGui::Separator();

    // FXAA
    auto* fxaa = engine_->postProcessStack().getEffect("FXAA");
    if (fxaa && ImGui::Checkbox("FXAA", &fxaaEnabled_)) {
        fxaa->setEnabled(fxaaEnabled_);
    }

    // Bloom
    auto* bloom = engine_->postProcessStack().getEffect("Bloom");
    if (bloom) {
        if (ImGui::Checkbox("Bloom", &bloomEnabled_)) {
            bloom->setEnabled(bloomEnabled_);
        }
        if (bloomEnabled_) {
            auto* bloomEffect = static_cast<Nox::BloomEffect*>(bloom);
            if (ImGui::SliderFloat("Bloom Threshold", &bloomThreshold_, 0.1f, 3.0f)) {
                bloomEffect->setThreshold(bloomThreshold_);
            }
            if (ImGui::SliderFloat("Bloom Intensity", &bloomIntensity_, 0.0f, 2.0f)) {
                bloomEffect->setIntensity(bloomIntensity_);
            }
        }
    }

    // SSAO
    auto* ssao = engine_->postProcessStack().getEffect("SSAO");
    if (ssao && ImGui::Checkbox("SSAO", &ssaoEnabled_)) {
        ssao->setEnabled(ssaoEnabled_);
    }
#endif
}

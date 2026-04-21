// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include "Level.hpp"

/// Demonstrates spatial audio with SDL3_mixer.
/// A sound source orbits around the listener (camera).
class LevelAudio : public Level {
public:
    LevelAudio() : Level("Spatial Audio") {}

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

    Nox::AudioSystem audioSystem_;
    std::shared_ptr<Nox::Mesh> soundMarker_;
    float orbitAngle_ = 0.0f;
    float orbitRadius_ = 5.0f;
    float orbitSpeed_ = 1.0f;
    float masterVolume_ = 1.0f;
    bool audioInitialized_ = false;
};

// ── Implementation ─────────────────────────────────────────────────

#ifdef NOX_HAS_IMGUI
#include <imgui.h>
#endif

inline void LevelAudio::setup([[maybe_unused]] Nox::Engine& engine) {
    using namespace Nox;

    camera_.setPosition(0.0f, 5.0f, 10.0f);
    camera_.lookAt(0.0f, 0.0f, 0.0f);

    // Lights
    auto sun = std::make_shared<DirectionalLight>(Color::White, 1.0f);
    sun->setDirection(-1.0f, -1.0f, -0.5f);
    scene_.add(sun);

    auto ambient = std::make_shared<AmbientLight>(Color::White, 0.15f);
    scene_.add(ambient);

    // Floor
    auto floorGeom = Geometry::plane(20.0f, 20.0f);
    auto floorMat = Material::standard();
    floorMat->setColor(Color(0.4f, 0.4f, 0.4f));
    auto floor = std::make_shared<Mesh>(floorGeom, floorMat);
    scene_.add(floor);

    // Listener marker (camera position)
    auto listenerGeom = Geometry::sphere(0.2f, 16, 8);
    auto listenerMat = Material::standard();
    listenerMat->setColor(Color(0.2f, 0.6f, 1.0f));
    auto listenerMarker = std::make_shared<Mesh>(listenerGeom, listenerMat);
    listenerMarker->setPosition(0.0f, 1.0f, 0.0f);
    scene_.add(listenerMarker);

    // Sound source marker (orbiting sphere)
    auto srcGeom = Geometry::sphere(0.3f, 16, 8);
    auto srcMat = Material::standard();
    srcMat->setColor(Color(1.0f, 0.3f, 0.1f));
    srcMat->setEmissiveColor(Color(1.0f, 0.3f, 0.1f));
    srcMat->setEmissiveIntensity(2.0f);
    soundMarker_ = std::make_shared<Mesh>(srcGeom, srcMat);
    soundMarker_->setPosition(orbitRadius_, 1.0f, 0.0f);
    scene_.add(soundMarker_);

    // Initialize audio
    audioInitialized_ = audioSystem_.init();
}

inline void LevelAudio::teardown([[maybe_unused]] Nox::Engine& engine) {
    audioSystem_.shutdown();
    soundMarker_.reset();
    scene_ = Nox::Scene3D();
    audioInitialized_ = false;
}

inline void LevelAudio::update(Nox::Engine& engine, float dt) {
    orbitCtrl_.update(engine.input(), dt);

    // Orbit the sound source around the origin
    orbitAngle_ += orbitSpeed_ * dt;
    float x = std::cos(orbitAngle_) * orbitRadius_;
    float z = std::sin(orbitAngle_) * orbitRadius_;
    soundMarker_->setPosition(x, 1.0f, z);

    // Update audio listener from camera
    if (audioInitialized_) {
        Nox::AudioListener listener;
        listener.position = camera_.position();
        listener.forward = glm::normalize(Nox::Math::Vec3(0.0f) - camera_.position());
        listener.up = { 0.0f, 1.0f, 0.0f };
        audioSystem_.setListener(listener);
        audioSystem_.setMasterGain(masterVolume_);
        audioSystem_.update();
    }
}

inline void LevelAudio::drawUI() {
#ifdef NOX_HAS_IMGUI
    ImGui::Text("Spatial Audio Demo");
    ImGui::Separator();
    if (audioInitialized_) {
        ImGui::Text("Audio: initialized");
    }
    else {
        ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Audio: not available");
    }
    ImGui::SliderFloat("Master Volume", &masterVolume_, 0.0f, 1.0f);
    ImGui::SliderFloat("Orbit Speed", &orbitSpeed_, 0.0f, 5.0f);
    ImGui::SliderFloat("Orbit Radius", &orbitRadius_, 1.0f, 20.0f);
    ImGui::Text("Sound source position: (%.1f, 1.0, %.1f)",
                std::cos(orbitAngle_) * orbitRadius_,
                std::sin(orbitAngle_) * orbitRadius_);
#endif
}

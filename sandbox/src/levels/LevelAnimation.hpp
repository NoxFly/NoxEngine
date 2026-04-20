// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include "Level.hpp"
#include <NoxEngine/animation/AnimationClip.hpp>
#include <NoxEngine/animation/AnimationPlayer.hpp>
#include <NoxEngine/animation/Skeleton.hpp>

class LevelAnimation : public Level {
public:
    LevelAnimation() : Level("Animation") {}

    void setup(Nox::Engine& engine) override;
    void teardown(Nox::Engine& engine) override;
    void update(Nox::Engine& engine, float dt) override;
    void drawUI() override;

    Nox::Scene3D& scene() override { return scene_; }
    Nox::PerspectiveCamera& camera() override { return camera_; }

private:
    void buildProceduralArm();
    void buildBounceAnimation();
    void buildWaveAnimation();

    Nox::Scene3D scene_;
    Nox::PerspectiveCamera camera_{ 45.0f, 1.77f, 0.1f, 1000.0f };
    Nox::OrbitCameraController orbitCtrl_{ camera_ };

    // Procedural arm made of linked segments
    static constexpr int ArmSegments = 4;
    std::shared_ptr<Nox::Mesh> armSegments_[ArmSegments];

    Nox::AnimationPlayer player_;
    std::shared_ptr<Nox::Skeleton> skeleton_;
    std::shared_ptr<Nox::AnimationClip> bounceClip_;
    std::shared_ptr<Nox::AnimationClip> waveClip_;

    float speed_ = 1.0f;
    bool playing_ = true;
    int currentClip_ = 0;
};

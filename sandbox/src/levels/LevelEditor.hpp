// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include "Level.hpp"

#include <NoxEngine/editor/AssetBrowser.hpp>
#include <NoxEngine/editor/Gizmo.hpp>
#include <NoxEngine/editor/PropertyInspector.hpp>
#include <NoxEngine/editor/SceneHierarchy.hpp>

class LevelEditor : public Level {
public:
    LevelEditor() : Level("Editor") {}

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

    Nox::SceneHierarchy hierarchy_;
    Nox::PropertyInspector inspector_;
    Nox::AssetBrowser assetBrowser_{ "." };
    Nox::Gizmo gizmo_;

    // Keep a reference to the engine for gizmo rendering
    Nox::Engine* engine_ = nullptr;
};

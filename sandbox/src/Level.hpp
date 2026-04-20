// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/NoxEngine.hpp>

#include <string>

// Base class for sandbox test levels.
class Level {
public:
    explicit Level(std::string name) : name_(std::move(name)) {}
    virtual ~Level() = default;

    Level(const Level&) = delete;
    Level& operator=(const Level&) = delete;
    Level(Level&&) = default;
    Level& operator=(Level&&) = default;

    [[nodiscard]] const std::string& name() const { return name_; }

    // Called once when the level is activated.
    virtual void setup(Nox::Engine& engine) = 0;

    // Called once when the level is deactivated (before the next level's setup).
    virtual void teardown(Nox::Engine& engine) = 0;

    // Called every frame. Returns the scene & camera to render.
    virtual void update(Nox::Engine& engine, float dt) = 0;

    // Access scene and camera for rendering.
    [[nodiscard]] virtual Nox::Scene3D& scene() = 0;
    [[nodiscard]] virtual Nox::PerspectiveCamera& camera() = 0;

    // Optional: draw ImGui controls specific to this level.
    virtual void drawUI() {}

private:
    std::string name_;
};

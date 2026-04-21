// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include "Level.hpp"

/// Demonstrates plugin system, Lua scripting, serialization, and custom render passes.
class LevelScripting : public Level {
public:
    LevelScripting() : Level("Scripting & Extensibility") {}

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

    Nox::LuaScriptEngine lua_;
    Nox::PluginSystem plugins_;
    bool luaInitialized_ = false;

    std::shared_ptr<Nox::Mesh> cube_;
    float rotationSpeed_ = 45.0f;

    // Custom render pass demo
    std::unique_ptr<Nox::CustomRenderPass> customPass_;

    // Serialization demo
    bool serialized_ = false;
    std::string serializationStatus_ = "Not tested";
};

// ── Implementation ─────────────────────────────────────────────────

#ifdef NOX_HAS_IMGUI
#include <imgui.h>
#endif

inline void LevelScripting::setup([[maybe_unused]] Nox::Engine& engine) {
    using namespace Nox;

    camera_.setPosition(3.0f, 3.0f, 5.0f);
    camera_.lookAt(0.0f, 0.0f, 0.0f);

    // Lights
    auto sun = std::make_shared<DirectionalLight>(Color::White, 1.0f);
    sun->setDirection(-1.0f, -1.0f, -0.5f);
    scene_.add(sun);

    auto ambient = std::make_shared<AmbientLight>(Color::White, 0.15f);
    scene_.add(ambient);

    // Floor
    auto floorGeom = Geometry::plane(10.0f, 10.0f);
    auto floorMat = Material::standard();
    floorMat->setColor(Color(0.5f, 0.5f, 0.5f));
    scene_.add(std::make_shared<Mesh>(floorGeom, floorMat));

    // Cube that will be manipulated by Lua
    auto cubeGeom = Geometry::box(1.0f, 1.0f, 1.0f);
    auto cubeMat = Material::standard();
    cubeMat->setColor(Color(0.2f, 0.8f, 0.3f));
    cube_ = std::make_shared<Mesh>(cubeGeom, cubeMat);
    cube_->setPosition(0.0f, 0.5f, 0.0f);
    scene_.add(cube_);

    // Initialize Lua scripting
    luaInitialized_ = lua_.init();
    if (luaInitialized_) {
        // Register a C++ function callable from Lua
        lua_.registerFunction("resetCube", [this]() {
            cube_->setPosition(0.0f, 0.5f, 0.0f);
            cube_->setRotation(0.0f, 0.0f, 0.0f);
        });

        // Execute inline Lua to set up the update function
        [[maybe_unused]] bool ok = lua_.execute(R"(
            angle = 0
            speed = 45

            function onUpdate(dt)
                angle = angle + speed * dt
            end
        )");
    }

    // Create a custom render pass (offscreen target)
    RenderPassDescriptor passDesc;
    passDesc.name = "ScriptingPassDemo";
    passDesc.width = 512;
    passDesc.height = 512;
    passDesc.colorAttachments = { { RenderPassAttachment::Format::RGBA8, true } };
    passDesc.hasDepthAttachment = true;
    customPass_ = std::make_unique<CustomRenderPass>(passDesc);
}

inline void LevelScripting::teardown([[maybe_unused]] Nox::Engine& engine) {
    lua_.shutdown();
    customPass_.reset();
    cube_.reset();
    scene_ = Nox::Scene3D();
}

inline void LevelScripting::update(Nox::Engine& engine, float dt) {
    orbitCtrl_.update(engine.input(), dt);

    // Drive cube rotation from Lua
    if (luaInitialized_) {
        lua_.callUpdate(dt);
        // Read back 'angle' from Lua and apply
        cube_->setRotation(0.0f, rotationSpeed_ * dt, 0.0f);
        cube_->rotate(0.0f, rotationSpeed_ * dt, 0.0f);
    }
}

inline void LevelScripting::drawUI() {
#ifdef NOX_HAS_IMGUI
    ImGui::Text("Scripting & Extensibility Demo");
    ImGui::Separator();

    // Lua status
    if (luaInitialized_) {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Lua: initialized");
        ImGui::SliderFloat("Rotation Speed", &rotationSpeed_, 0.0f, 360.0f);

        if (ImGui::Button("Execute Lua: resetCube()")) {
            lua_.callFunction("resetCube");
        }
    }
    else {
        ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Lua: not available");
    }

    ImGui::Separator();

    // Plugin system info
    ImGui::Text("Plugin System: ready");
    ImGui::Text("Loaded plugins: %zu", plugins_.pluginNames().size());

    ImGui::Separator();

    // Custom render pass info
    if (customPass_) {
        ImGui::Text("Custom Render Pass: '%s' (%ux%u)",
                     customPass_->name().c_str(),
                     customPass_->width(),
                     customPass_->height());
    }

    ImGui::Separator();

    // Serialization test
    if (ImGui::Button("Test Serialization")) {
        Nox::Archive archive;
        archive.write(42);
        archive.write(3.14f);
        archive.writeString("NoxEngine");

        archive.resetRead();
        int intVal = 0;
        float floatVal = 0.0f;
        archive.read(intVal);
        archive.read(floatVal);
        std::string strVal = archive.readString();

        if (intVal == 42 && std::abs(floatVal - 3.14f) < 0.001f && strVal == "NoxEngine") {
            serializationStatus_ = "PASS: Archive read/write works correctly";
            serialized_ = true;
        }
        else {
            serializationStatus_ = "FAIL: Data mismatch";
            serialized_ = false;
        }
    }
    if (serialized_) {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "%s", serializationStatus_.c_str());
    }
    else {
        ImGui::Text("%s", serializationStatus_.c_str());
    }
#endif
}

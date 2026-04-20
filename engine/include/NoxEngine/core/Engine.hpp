// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/platform/Window.hpp>
#include <NoxEngine/renderer/Camera.hpp>
#include <NoxEngine/scene/Scene3D.hpp>

#include <functional>
#include <memory>
#include <string>
#include <string_view>

namespace Nox {

    class Renderer;

    struct EngineConfig {
        std::string title  = "NoxEngine";
        int         width  = 1280;
        int         height = 720;
        bool        vsync  = true;
    };

    class Engine {
    public:
        explicit Engine(const EngineConfig& config);
        ~Engine();

        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;
        Engine(Engine&&) = delete;
        Engine& operator=(Engine&&) = delete;

        void run(std::function<void(float dt)> loopFn);
        void render(Scene3D& scene, PerspectiveCamera& camera);

        [[nodiscard]] float aspect() const;
        [[nodiscard]] Window& window() { return *window_; }
        [[nodiscard]] const Window& window() const { return *window_; }

    private:
        void uploadMesh(Mesh& mesh);

        std::unique_ptr<Window>   window_;
        std::unique_ptr<Renderer> renderer_;
        uint32_t defaultPipeline_ = 0;
        bool     pipelineReady_   = false;
    };

} // namespace Nox

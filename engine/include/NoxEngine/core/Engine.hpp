// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/core/DebugOverlay.hpp>
#include <NoxEngine/platform/Window.hpp>
#include <NoxEngine/renderer/Camera.hpp>
#include <NoxEngine/scene/Scene3D.hpp>
#include <NoxEngine/scene/SceneNode.hpp>

#include <filesystem>
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
        void render(Scene3D& scene, OrthographicCamera& camera);

        [[nodiscard]] std::shared_ptr<SceneNode> load(const std::filesystem::path& path);

        [[nodiscard]] float aspect() const;
        [[nodiscard]] Window& window() { return *window_; }
        [[nodiscard]] const Window& window() const { return *window_; }
        [[nodiscard]] float fps() const { return currentFps_; }
        [[nodiscard]] float frameTime() const { return currentFrameTime_; }
        [[nodiscard]] DebugOverlay& debugOverlay() { return debugOverlay_; }

    private:
        void uploadMesh(Mesh& mesh);
        void uploadTexture(Mesh& mesh);
        void renderInternal(Scene3D& scene, const Math::Mat4& viewMatrix,
                            const Math::Mat4& projMatrix, const Math::Vec3& cameraPos);

        std::unique_ptr<Window>   window_;
        std::unique_ptr<Renderer> renderer_;
        uint32_t litPipeline_   = 0;
        uint32_t unlitPipeline_ = 0;
        bool     pipelineReady_ = false;
        float    currentFps_       = 0.0f;
        float    currentFrameTime_ = 0.0f;
        DebugOverlay debugOverlay_;
    };

} // namespace Nox

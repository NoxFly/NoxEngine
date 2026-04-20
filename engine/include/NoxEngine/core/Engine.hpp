// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/core/AssetCache.hpp>
#include <NoxEngine/core/DebugOverlay.hpp>
#include <NoxEngine/core/FileWatcher.hpp>
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
        void stop();
        void render(Scene3D& scene, PerspectiveCamera& camera);
        void render(Scene3D& scene, OrthographicCamera& camera);

        [[nodiscard]] std::shared_ptr<SceneNode> load(const std::filesystem::path& path);

        [[nodiscard]] float aspect() const;
        [[nodiscard]] Window& window() { return *window_; }
        [[nodiscard]] const Window& window() const { return *window_; }
        [[nodiscard]] float fps() const { return currentFps_; }
        [[nodiscard]] float frameTime() const { return currentFrameTime_; }
        [[nodiscard]] DebugOverlay& debugOverlay() { return debugOverlay_; }
        [[nodiscard]] Input& input() { return window_->input(); }
        [[nodiscard]] const Input& input() const { return window_->input(); }

        void setShaderDirectory(const std::filesystem::path& dir);
        void setExposure(float e) { exposure_ = e; }
        [[nodiscard]] float exposure() const { return exposure_; }

    private:
        void uploadMesh(Mesh& mesh);
        void uploadTexture(Mesh& mesh);
        uint32_t loadTexture(const std::filesystem::path& path);
        void renderInternal(Scene3D& scene, const Math::Mat4& viewMatrix,
                            const Math::Mat4& projMatrix, const Math::Vec3& cameraPos);
        void rebuildPipelines();
        void createShadowResources();
        void createHDRResources(int width, int height);
        void renderToneMapPass();

        std::unique_ptr<Window>   window_;
        std::unique_ptr<Renderer> renderer_;
        uint32_t litPipeline_   = 0;
        uint32_t unlitPipeline_ = 0;
        bool     pipelineReady_ = false;
        float    currentFps_       = 0.0f;
        float    currentFrameTime_ = 0.0f;
        bool     running_          = false;
        DebugOverlay debugOverlay_;
        AssetCache<uint32_t> textureCache_;
        FileWatcher shaderWatcher_;
        std::filesystem::path shaderDir_;

        // Shadow mapping
        uint32_t shadowPipeline_  = 0;
        uint32_t shadowFBO_       = 0;
        uint32_t shadowDepthTex_  = 0;
        static constexpr int ShadowMapSize = 2048;
        Math::Mat4 lightSpaceMatrix_{ 1.0f };

        // Point light shadow mapping
        uint32_t pointShadowPipeline_ = 0;
        static constexpr int MaxShadowPointLights = 4;
        static constexpr int PointShadowMapSize = 1024;
        uint32_t pointShadowFBOs_[MaxShadowPointLights] = {};
        uint32_t pointShadowCubemaps_[MaxShadowPointLights] = {};
        int      numShadowPointLights_ = 0;

        // HDR rendering
        uint32_t hdrFBO_         = 0;
        uint32_t hdrColorTex_    = 0;
        uint32_t hdrDepthRBO_    = 0;
        uint32_t toneMapPipeline_ = 0;
        uint32_t screenQuadVAO_  = 0;
        uint32_t screenQuadVBO_  = 0;
        int      hdrWidth_       = 0;
        int      hdrHeight_      = 0;
        float    exposure_       = 1.0f;
    };

} // namespace Nox

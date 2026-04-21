// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>
#include <NoxEngine/renderer/PostProcessStack.hpp>

#include <array>
#include <cstdint>

namespace Nox {

    /// Owns all GPU rendering state: pipelines, shadow maps, HDR framebuffer,
    /// and the post-process stack.
    /// Extracted from Engine to keep it focused on orchestration.
    /// RAII: destructor cleans up all GPU resources.
    struct RenderContext {
        RenderContext() = default;
        ~RenderContext();

        RenderContext(const RenderContext&) = delete;
        RenderContext& operator=(const RenderContext&) = delete;
        RenderContext(RenderContext&&) = delete;
        RenderContext& operator=(RenderContext&&) = delete;

        // ── Wireframe mode (engine-level, no raw GL in user code) ──
        void setWireframe(bool enabled);
        [[nodiscard]] bool wireframe() const { return wireframe_; }

        // ── Pipelines ──────────────────────────────────────────────
        uint32_t litPipeline        = 0;
        uint32_t unlitPipeline      = 0;
        uint32_t shadowPipeline     = 0;
        uint32_t pointShadowPipeline = 0;
        uint32_t toneMapPipeline    = 0;
        bool     pipelineReady      = false;

        // ── Directional shadow map ─────────────────────────────────
        static constexpr int ShadowMapSize = 2048;
        uint32_t shadowFBO       = 0;
        uint32_t shadowDepthTex  = 0;
        Math::Mat4 lightSpaceMatrix{ 1.0f };

        // ── Point light shadow maps (std::array, RAII-safe) ────────
        static constexpr int MaxShadowPointLights = 4;
        static constexpr int PointShadowMapSize   = 1024;
        std::array<uint32_t, MaxShadowPointLights> pointShadowFBOs{};
        std::array<uint32_t, MaxShadowPointLights> pointShadowCubemaps{};
        int numShadowPointLights = 0;

        // ── HDR framebuffer ────────────────────────────────────────
        uint32_t hdrFBO         = 0;
        uint32_t hdrColorTex    = 0;
        uint32_t hdrDepthTex    = 0;
        uint32_t screenQuadVAO  = 0;
        uint32_t screenQuadVBO  = 0;
        int      hdrWidth       = 0;
        int      hdrHeight      = 0;
        float    exposure       = 1.0f;

        // ── Post-processing ────────────────────────────────────────
        PostProcessStack postProcessStack;

    private:
        bool wireframe_ = false;
    };

} // namespace Nox

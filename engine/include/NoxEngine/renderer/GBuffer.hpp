// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

#include <cstdint>

namespace Nox {

    /// G-Buffer for deferred rendering.
    /// Stores per-pixel geometry data in multiple render targets:
    ///   RT0 (RGBA16F): World-space position (xyz) + metallic (w)
    ///   RT1 (RGBA16F): World-space normal (xyz) + roughness (w)
    ///   RT2 (RGBA8):   Albedo (rgb) + AO (a)
    ///   RT3 (RGBA16F): Emissive (rgb) + unused (a)
    ///   Depth:         Depth buffer (24-bit)
    class GBuffer {
    public:
        GBuffer() = default;
        ~GBuffer();

        GBuffer(const GBuffer&) = delete;
        GBuffer& operator=(const GBuffer&) = delete;
        GBuffer(GBuffer&&) noexcept;
        GBuffer& operator=(GBuffer&&) noexcept;

        /// Create or recreate G-Buffer with given dimensions.
        void init(int width, int height);

        /// Resize all attachments.
        void resize(int width, int height);

        /// Bind the G-Buffer FBO for geometry pass writing.
        void bindForWriting() const;

        /// Bind the G-Buffer textures for lighting pass reading.
        /// Binds position to unit 0, normal to 1, albedo to 2, emissive to 3, depth to 4.
        void bindForReading() const;

        /// Unbind (bind default FBO).
        static void unbind();

        [[nodiscard]] uint32_t fbo() const { return fbo_; }
        [[nodiscard]] uint32_t positionTexture() const { return positionTex_; }
        [[nodiscard]] uint32_t normalTexture() const { return normalTex_; }
        [[nodiscard]] uint32_t albedoTexture() const { return albedoTex_; }
        [[nodiscard]] uint32_t emissiveTexture() const { return emissiveTex_; }
        [[nodiscard]] uint32_t depthTexture() const { return depthTex_; }
        [[nodiscard]] int width() const { return width_; }
        [[nodiscard]] int height() const { return height_; }
        [[nodiscard]] bool isInitialized() const { return initialized_; }

    private:
        void destroy();

        uint32_t fbo_          = 0;
        uint32_t positionTex_  = 0;
        uint32_t normalTex_    = 0;
        uint32_t albedoTex_    = 0;
        uint32_t emissiveTex_  = 0;
        uint32_t depthTex_     = 0;
        int      width_        = 0;
        int      height_       = 0;
        bool     initialized_  = false;
    };

} // namespace Nox

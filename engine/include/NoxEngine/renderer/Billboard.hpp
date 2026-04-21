// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>
#include <NoxEngine/scene/SceneObject.hpp>

#include <cstdint>
#include <memory>
#include <string>

namespace Nox {

    class Geometry;
    class Material;

    /// A billboard is a quad that always faces the camera.
    /// Useful for particles, labels, sprites, and UI in 3D space.
    class Billboard : public SceneObject {
    public:
        explicit Billboard(float width = 1.0f, float height = 1.0f);
        ~Billboard() override = default;

        void setSize(float width, float height);
        void setTexture(const std::string& path);

        [[nodiscard]] float width() const { return width_; }
        [[nodiscard]] float height() const { return height_; }
        [[nodiscard]] const std::string& texturePath() const { return texturePath_; }

        [[nodiscard]] bool gpuReady() const { return gpuReady_; }
        void markGpuReady(bool ready) { gpuReady_ = ready; }

        /// GPU data for rendering.
        struct GpuData {
            uint32_t vao = 0;
            uint32_t vbo = 0;
            uint32_t textureId = 0;
        };

        GpuData& gpuData() { return gpuData_; }
        [[nodiscard]] const GpuData& gpuData() const { return gpuData_; }

    private:
        float width_  = 1.0f;
        float height_ = 1.0f;
        std::string texturePath_;
        bool gpuReady_ = false;
        GpuData gpuData_{};
    };

    /// Simple text rendering using a bitmap font atlas.
    /// Renders text as textured quads in screen space.
    class TextRenderer {
    public:
        TextRenderer();
        ~TextRenderer();

        TextRenderer(const TextRenderer&) = delete;
        TextRenderer& operator=(const TextRenderer&) = delete;
        TextRenderer(TextRenderer&&) = default;
        TextRenderer& operator=(TextRenderer&&) = default;

        /// Initialize the text rendering system with a built-in bitmap font.
        void init();

        /// Render text at screen-space pixel coordinates.
        void drawText(const std::string& text, float x, float y, float scale = 1.0f,
                      const Math::Vec3& color = Math::Vec3(1.0f));

        /// Render text at screen-space coordinates with viewport dimensions.
        void drawText(const std::string& text, float x, float y,
                      int viewportWidth, int viewportHeight,
                      float scale = 1.0f, const Math::Vec3& color = Math::Vec3(1.0f));

        [[nodiscard]] bool isInitialized() const { return initialized_; }

    private:
        void createFontAtlas();

        struct CharInfo {
            float u0, v0, u1, v1;   // UV coordinates in the atlas
            float width, height;     // Character size in pixels
            float xOffset, yOffset;  // Offset for glyph placement
            float advance;           // Horizontal advance
        };

        uint32_t fontAtlasTexture_ = 0;
        uint32_t textVAO_          = 0;
        uint32_t textVBO_          = 0;
        uint32_t textProgram_      = 0;
        bool     initialized_      = false;
        CharInfo charMap_[128]{};
        float    atlasWidth_  = 0.0f;
        float    atlasHeight_ = 0.0f;
    };

    /// Simple UI system for in-engine 2D elements (not ImGui).
    /// Provides basic rectangles and labels in screen space.
    class UISystem {
    public:
        UISystem() = default;
        ~UISystem();

        UISystem(const UISystem&) = delete;
        UISystem& operator=(const UISystem&) = delete;
        UISystem(UISystem&&) = default;
        UISystem& operator=(UISystem&&) = default;

        /// Initialize the UI rendering pipeline.
        void init();

        /// Draw a filled rectangle in screen space (pixel coordinates).
        void drawRect(float x, float y, float width, float height,
                      const Math::Vec4& color, int viewportWidth, int viewportHeight);

        /// Begin a new UI frame (clear queued commands).
        void beginFrame();

        /// Flush all queued UI draw commands.
        void endFrame(int viewportWidth, int viewportHeight);

        [[nodiscard]] bool isInitialized() const { return initialized_; }

    private:
        struct UIRect {
            float x, y, width, height;
            Math::Vec4 color;
        };

        uint32_t uiProgram_ = 0;
        uint32_t uiVAO_     = 0;
        uint32_t uiVBO_     = 0;
        bool     initialized_ = false;
        std::vector<UIRect> pendingRects_;
    };

} // namespace Nox

// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace Nox {

    class Scene3D;
    class PerspectiveCamera;

    /// Describes a framebuffer attachment for a custom render pass.
    struct RenderPassAttachment {
        enum class Format : uint8_t {
            RGBA8,
            RGBA16F,
            Depth24Stencil8,
            Depth32F
        };
        Format format = Format::RGBA8;
        bool clear = true;
    };

    /// Descriptor for creating a custom render pass.
    struct RenderPassDescriptor {
        std::string name;
        uint32_t width = 0;    ///< 0 = use window width
        uint32_t height = 0;   ///< 0 = use window height
        std::vector<RenderPassAttachment> colorAttachments;
        bool hasDepthAttachment = true;
    };

    /// A custom render pass that users can inject into the rendering pipeline.
    /// Owns its own framebuffer and can render the scene with custom settings.
    class CustomRenderPass {
    public:
        explicit CustomRenderPass(const RenderPassDescriptor& desc);
        ~CustomRenderPass();

        CustomRenderPass(const CustomRenderPass&) = delete;
        CustomRenderPass& operator=(const CustomRenderPass&) = delete;
        CustomRenderPass(CustomRenderPass&&) noexcept;
        CustomRenderPass& operator=(CustomRenderPass&&) noexcept;

        /// Bind this render pass as the active framebuffer target.
        void begin();

        /// Unbind and restore default framebuffer.
        void end();

        /// Resize the framebuffer attachments.
        void resize(uint32_t width, uint32_t height);

        /// Get the color texture ID for a given attachment index (for sampling).
        [[nodiscard]] uint32_t colorTexture(uint32_t index = 0) const;

        /// Get the depth texture ID.
        [[nodiscard]] uint32_t depthTexture() const;

        [[nodiscard]] const std::string& name() const { return name_; }
        [[nodiscard]] uint32_t width() const { return width_; }
        [[nodiscard]] uint32_t height() const { return height_; }

    private:
        void createResources();
        void destroyResources();

        std::string name_;
        uint32_t width_ = 0;
        uint32_t height_ = 0;
        uint32_t fbo_ = 0;
        std::vector<uint32_t> colorTextures_;
        uint32_t depthTexture_ = 0;
        bool hasDepth_ = true;
        std::vector<RenderPassAttachment> attachments_;
    };

} // namespace Nox

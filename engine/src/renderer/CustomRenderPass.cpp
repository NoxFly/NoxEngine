// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/CustomRenderPass.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

namespace Nox {

    namespace {
        [[nodiscard]] GLenum toGlInternalFormat(RenderPassAttachment::Format fmt) {
            switch (fmt) {
                case RenderPassAttachment::Format::RGBA8:            return GL_RGBA8;
                case RenderPassAttachment::Format::RGBA16F:          return GL_RGBA16F;
                case RenderPassAttachment::Format::Depth24Stencil8:  return GL_DEPTH24_STENCIL8;
                case RenderPassAttachment::Format::Depth32F:         return GL_DEPTH_COMPONENT32F;
            }
            return GL_RGBA8;
        }
    }

    CustomRenderPass::CustomRenderPass(const RenderPassDescriptor& desc)
        : name_(desc.name)
        , width_(desc.width)
        , height_(desc.height)
        , hasDepth_(desc.hasDepthAttachment)
        , attachments_(desc.colorAttachments)
    {
        if (width_ == 0) { width_ = 1920; }
        if (height_ == 0) { height_ = 1080; }
        createResources();
    }

    CustomRenderPass::~CustomRenderPass() {
        destroyResources();
    }

    CustomRenderPass::CustomRenderPass(CustomRenderPass&& other) noexcept
        : name_(std::move(other.name_))
        , width_(other.width_)
        , height_(other.height_)
        , fbo_(other.fbo_)
        , colorTextures_(std::move(other.colorTextures_))
        , depthTexture_(other.depthTexture_)
        , hasDepth_(other.hasDepth_)
        , attachments_(std::move(other.attachments_))
    {
        other.fbo_ = 0;
        other.depthTexture_ = 0;
    }

    CustomRenderPass& CustomRenderPass::operator=(CustomRenderPass&& other) noexcept {
        if (this != &other) {
            destroyResources();
            name_ = std::move(other.name_);
            width_ = other.width_;
            height_ = other.height_;
            fbo_ = other.fbo_;
            colorTextures_ = std::move(other.colorTextures_);
            depthTexture_ = other.depthTexture_;
            hasDepth_ = other.hasDepth_;
            attachments_ = std::move(other.attachments_);
            other.fbo_ = 0;
            other.depthTexture_ = 0;
        }
        return *this;
    }

    void CustomRenderPass::begin() {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glViewport(0, 0, static_cast<GLsizei>(width_), static_cast<GLsizei>(height_));

        GLbitfield clearBits = 0;
        for (size_t i = 0; i < attachments_.size(); ++i) {
            if (attachments_[i].clear) {
                clearBits |= GL_COLOR_BUFFER_BIT;
                break;
            }
        }
        if (hasDepth_) { clearBits |= GL_DEPTH_BUFFER_BIT; }
        if (clearBits != 0) { glClear(clearBits); }
    }

    void CustomRenderPass::end() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void CustomRenderPass::resize(uint32_t width, uint32_t height) {
        if (width == width_ && height == height_) { return; }
        width_ = width;
        height_ = height;
        destroyResources();
        createResources();
    }

    uint32_t CustomRenderPass::colorTexture(uint32_t index) const {
        if (index >= colorTextures_.size()) { return 0; }
        return colorTextures_[index];
    }

    uint32_t CustomRenderPass::depthTexture() const {
        return depthTexture_;
    }

    void CustomRenderPass::createResources() {
        glCreateFramebuffers(1, &fbo_);

        colorTextures_.resize(attachments_.size());

        for (size_t i = 0; i < attachments_.size(); ++i) {
            GLuint tex = 0;
            glCreateTextures(GL_TEXTURE_2D, 1, &tex);
            glTextureStorage2D(tex, 1, toGlInternalFormat(attachments_[i].format),
                               static_cast<GLsizei>(width_), static_cast<GLsizei>(height_));
            glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glNamedFramebufferTexture(fbo_, static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i), tex, 0);
            colorTextures_[i] = tex;
        }

        if (hasDepth_) {
            glCreateTextures(GL_TEXTURE_2D, 1, &depthTexture_);
            glTextureStorage2D(depthTexture_, 1, GL_DEPTH_COMPONENT32F,
                               static_cast<GLsizei>(width_), static_cast<GLsizei>(height_));
            glNamedFramebufferTexture(fbo_, GL_DEPTH_ATTACHMENT, depthTexture_, 0);
        }

        // Set draw buffers
        if (!attachments_.empty()) {
            std::vector<GLenum> drawBuffers;
            drawBuffers.reserve(attachments_.size());
            for (size_t i = 0; i < attachments_.size(); ++i) {
                drawBuffers.push_back(static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i));
            }
            glNamedFramebufferDrawBuffers(fbo_, static_cast<GLsizei>(drawBuffers.size()),
                                           drawBuffers.data());
        }

        GLenum status = glCheckNamedFramebufferStatus(fbo_, GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            NOX_LOG_ERROR("CustomRenderPass '{}': framebuffer incomplete (status=0x{:X})",
                          name_, status);
        }
    }

    void CustomRenderPass::destroyResources() {
        for (uint32_t tex : colorTextures_) {
            if (tex != 0) { glDeleteTextures(1, &tex); }
        }
        colorTextures_.clear();

        if (depthTexture_ != 0) {
            glDeleteTextures(1, &depthTexture_);
            depthTexture_ = 0;
        }

        if (fbo_ != 0) {
            glDeleteFramebuffers(1, &fbo_);
            fbo_ = 0;
        }
    }

} // namespace Nox

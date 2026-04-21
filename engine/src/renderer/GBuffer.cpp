// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/GBuffer.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

#include <utility>

namespace Nox {

    GBuffer::~GBuffer() {
        destroy();
    }

    GBuffer::GBuffer(GBuffer&& other) noexcept
        : fbo_(other.fbo_)
        , positionTex_(other.positionTex_)
        , normalTex_(other.normalTex_)
        , albedoTex_(other.albedoTex_)
        , emissiveTex_(other.emissiveTex_)
        , depthTex_(other.depthTex_)
        , width_(other.width_)
        , height_(other.height_)
        , initialized_(other.initialized_) {
        other.fbo_ = 0;
        other.positionTex_ = 0;
        other.normalTex_ = 0;
        other.albedoTex_ = 0;
        other.emissiveTex_ = 0;
        other.depthTex_ = 0;
        other.initialized_ = false;
    }

    GBuffer& GBuffer::operator=(GBuffer&& other) noexcept {
        if (this != &other) {
            destroy();
            fbo_ = other.fbo_;
            positionTex_ = other.positionTex_;
            normalTex_ = other.normalTex_;
            albedoTex_ = other.albedoTex_;
            emissiveTex_ = other.emissiveTex_;
            depthTex_ = other.depthTex_;
            width_ = other.width_;
            height_ = other.height_;
            initialized_ = other.initialized_;
            other.fbo_ = 0;
            other.positionTex_ = 0;
            other.normalTex_ = 0;
            other.albedoTex_ = 0;
            other.emissiveTex_ = 0;
            other.depthTex_ = 0;
            other.initialized_ = false;
        }
        return *this;
    }

    void GBuffer::init(int width, int height) {
        if (initialized_) {
            destroy();
        }

        width_ = width;
        height_ = height;

        // Create FBO
        glCreateFramebuffers(1, &fbo_);

        // RT0: Position (xyz) + Metallic (w)
        glCreateTextures(GL_TEXTURE_2D, 1, &positionTex_);
        glTextureStorage2D(positionTex_, 1, GL_RGBA16F, width, height);
        glTextureParameteri(positionTex_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(positionTex_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(positionTex_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(positionTex_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // RT1: Normal (xyz) + Roughness (w)
        glCreateTextures(GL_TEXTURE_2D, 1, &normalTex_);
        glTextureStorage2D(normalTex_, 1, GL_RGBA16F, width, height);
        glTextureParameteri(normalTex_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(normalTex_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(normalTex_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(normalTex_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // RT2: Albedo (rgb) + AO (a)
        glCreateTextures(GL_TEXTURE_2D, 1, &albedoTex_);
        glTextureStorage2D(albedoTex_, 1, GL_RGBA8, width, height);
        glTextureParameteri(albedoTex_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(albedoTex_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(albedoTex_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(albedoTex_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // RT3: Emissive (rgb) + unused (a)
        glCreateTextures(GL_TEXTURE_2D, 1, &emissiveTex_);
        glTextureStorage2D(emissiveTex_, 1, GL_RGBA16F, width, height);
        glTextureParameteri(emissiveTex_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(emissiveTex_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(emissiveTex_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(emissiveTex_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Depth
        glCreateTextures(GL_TEXTURE_2D, 1, &depthTex_);
        glTextureStorage2D(depthTex_, 1, GL_DEPTH_COMPONENT24, width, height);
        glTextureParameteri(depthTex_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(depthTex_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(depthTex_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(depthTex_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Attach to FBO
        glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0, positionTex_, 0);
        glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT1, normalTex_, 0);
        glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT2, albedoTex_, 0);
        glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT3, emissiveTex_, 0);
        glNamedFramebufferTexture(fbo_, GL_DEPTH_ATTACHMENT, depthTex_, 0);

        GLenum drawBuffers[4] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2,
            GL_COLOR_ATTACHMENT3
        };
        glNamedFramebufferDrawBuffers(fbo_, 4, drawBuffers);

        // Check completeness
        GLenum status = glCheckNamedFramebufferStatus(fbo_, GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            NOX_LOG_ERROR("GBuffer FBO incomplete: 0x{:X}", status);
            destroy();
            return;
        }

        initialized_ = true;
        NOX_LOG_INFO("GBuffer initialized ({}x{})", width, height);
    }

    void GBuffer::resize(int width, int height) {
        if (width == width_ && height == height_) { return; }
        init(width, height);
    }

    void GBuffer::bindForWriting() const {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glViewport(0, 0, width_, height_);
    }

    void GBuffer::bindForReading() const {
        glBindTextureUnit(0, positionTex_);
        glBindTextureUnit(1, normalTex_);
        glBindTextureUnit(2, albedoTex_);
        glBindTextureUnit(3, emissiveTex_);
        glBindTextureUnit(4, depthTex_);
    }

    void GBuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void GBuffer::destroy() {
        if (fbo_ != 0)         { glDeleteFramebuffers(1, &fbo_); fbo_ = 0; }
        if (positionTex_ != 0) { glDeleteTextures(1, &positionTex_); positionTex_ = 0; }
        if (normalTex_ != 0)   { glDeleteTextures(1, &normalTex_); normalTex_ = 0; }
        if (albedoTex_ != 0)   { glDeleteTextures(1, &albedoTex_); albedoTex_ = 0; }
        if (emissiveTex_ != 0) { glDeleteTextures(1, &emissiveTex_); emissiveTex_ = 0; }
        if (depthTex_ != 0)    { glDeleteTextures(1, &depthTex_); depthTex_ = 0; }
        initialized_ = false;
    }

} // namespace Nox

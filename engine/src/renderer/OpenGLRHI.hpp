// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/renderer/RHI.hpp>
#include <NoxEngine/core/HandlePool.hpp>

#include <GL/glew.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace Nox {

    // ── Internal resource records ──────────────────────────────────
    struct GLBufferData {
        GLuint      id    = 0;
        BufferUsage usage = BufferUsage::Vertex;
        uint32_t    size  = 0;
    };

    struct GLTextureData {
        GLuint        id     = 0;
        uint32_t      width  = 0;
        uint32_t      height = 0;
        TextureFormat format = TextureFormat::RGBA8;
    };

    struct GLShaderData {
        GLuint      id    = 0;
        ShaderStage stage = ShaderStage::Vertex;
    };

    struct GLPipelineData {
        GLuint program = 0;
        GLuint vao     = 0;
        std::unordered_map<std::string, GLint> uniformLocations;
    };

    // ── OpenGL CommandBuffer ───────────────────────────────────────
    class OpenGLCommandBuffer : public CommandBuffer {
    public:
        explicit OpenGLCommandBuffer(
            HandlePool<GLBufferData>&   buffers,
            HandlePool<GLPipelineData>& pipelines
        );

        void setViewport(int x, int y, int width, int height) override;
        void clear(float r, float g, float b, float a, float depth) override;
        void bindPipeline(PipelineHandle pipeline) override;
        void bindVertexBuffer(BufferHandle buffer) override;
        void bindIndexBuffer(BufferHandle buffer) override;
        void pushConstant(std::string_view name, const Math::Mat4& value) override;
        void pushConstant(std::string_view name, const Math::Vec3& value) override;
        void pushConstant(std::string_view name, float value) override;
        void pushConstant(std::string_view name, int value) override;
        void drawIndexed(uint32_t indexCount) override;
        void drawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount) override;

    private:
        [[nodiscard]] GLint getUniformLocation(std::string_view name) const;

        HandlePool<GLBufferData>&   buffers_;
        HandlePool<GLPipelineData>& pipelines_;
        GLPipelineData*             boundPipeline_ = nullptr;
    };

    // ── OpenGL RHI ─────────────────────────────────────────────────
    class OpenGLRHI : public RHI {
    public:
        OpenGLRHI();
        ~OpenGLRHI() override;

        [[nodiscard]] BufferHandle   createBuffer(const BufferDesc& desc) override;
        [[nodiscard]] TextureHandle  createTexture(const TextureDesc& desc) override;
        [[nodiscard]] ShaderHandle   createShader(const ShaderDesc& desc) override;
        [[nodiscard]] PipelineHandle createPipeline(const PipelineDesc& desc) override;

        void destroyBuffer(BufferHandle handle) override;
        void destroyTexture(TextureHandle handle) override;
        void destroyShader(ShaderHandle handle) override;
        void destroyPipeline(PipelineHandle handle) override;

        [[nodiscard]] CommandBuffer* beginFrame() override;
        void endFrame(CommandBuffer* cmd) override;

        HandlePool<GLBufferData>&   bufferPool()   { return buffers_; }
        HandlePool<GLPipelineData>& pipelinePool() { return pipelines_; }

    private:
        HandlePool<GLBufferData>    buffers_;
        HandlePool<GLTextureData>   textures_;
        HandlePool<GLShaderData>    shaders_;
        HandlePool<GLPipelineData>  pipelines_;

        std::unique_ptr<OpenGLCommandBuffer> cmdBuffer_;
    };

} // namespace Nox

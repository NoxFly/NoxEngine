// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

#include <cstdint>
#include <memory>
#include <span>
#include <string_view>

namespace Nox {

    // ── Resource handles ───────────────────────────────────────────
    struct BufferHandle   { uint32_t index = 0; uint32_t generation = 0; [[nodiscard]] bool valid() const { return generation != 0; } };
    struct TextureHandle  { uint32_t index = 0; uint32_t generation = 0; [[nodiscard]] bool valid() const { return generation != 0; } };
    struct ShaderHandle   { uint32_t index = 0; uint32_t generation = 0; [[nodiscard]] bool valid() const { return generation != 0; } };
    struct PipelineHandle { uint32_t index = 0; uint32_t generation = 0; [[nodiscard]] bool valid() const { return generation != 0; } };

    // ── Descriptor structs ─────────────────────────────────────────
    enum class BufferUsage : uint8_t { Vertex, Index, Uniform };

    struct BufferDesc {
        BufferUsage usage     = BufferUsage::Vertex;
        uint32_t    sizeBytes = 0;
        const void* data      = nullptr;
    };

    enum class TextureFormat : uint8_t { RGBA8, RGB8, Depth24Stencil8 };

    struct TextureDesc {
        uint32_t      width  = 1;
        uint32_t      height = 1;
        TextureFormat format = TextureFormat::RGBA8;
        const void*   data   = nullptr;
    };

    enum class ShaderStage : uint8_t { Vertex, Fragment, Compute };

    struct ShaderDesc {
        ShaderStage      stage  = ShaderStage::Vertex;
        std::string_view source;
    };

    struct PipelineDesc {
        ShaderHandle vertexShader{};
        ShaderHandle fragmentShader{};
        bool         depthTest   = true;
        bool         depthWrite  = true;
        bool         blending    = false;
    };

    // ── CommandBuffer ──────────────────────────────────────────────
    class CommandBuffer {
    public:
        virtual ~CommandBuffer() = default;

        virtual void setViewport(int x, int y, int width, int height) = 0;
        virtual void clear(float r, float g, float b, float a, float depth) = 0;
        virtual void bindPipeline(PipelineHandle pipeline) = 0;
        virtual void bindVertexBuffer(BufferHandle buffer) = 0;
        virtual void bindIndexBuffer(BufferHandle buffer) = 0;
        virtual void pushConstant(std::string_view name, const Math::Mat4& value) = 0;
        virtual void pushConstant(std::string_view name, const Math::Vec3& value) = 0;
        virtual void pushConstant(std::string_view name, float value) = 0;
        virtual void pushConstant(std::string_view name, int value) = 0;
        virtual void drawIndexed(uint32_t indexCount) = 0;
        virtual void drawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount) = 0;
    };

    // ── RHI abstract interface ─────────────────────────────────────
    class RHI {
    public:
        virtual ~RHI() = default;

        [[nodiscard]] virtual BufferHandle   createBuffer(const BufferDesc& desc) = 0;
        [[nodiscard]] virtual TextureHandle  createTexture(const TextureDesc& desc) = 0;
        [[nodiscard]] virtual ShaderHandle   createShader(const ShaderDesc& desc) = 0;
        [[nodiscard]] virtual PipelineHandle createPipeline(const PipelineDesc& desc) = 0;

        virtual void destroyBuffer(BufferHandle handle) = 0;
        virtual void destroyTexture(TextureHandle handle) = 0;
        virtual void destroyShader(ShaderHandle handle) = 0;
        virtual void destroyPipeline(PipelineHandle handle) = 0;

        [[nodiscard]] virtual CommandBuffer* beginFrame() = 0;
        virtual void endFrame(CommandBuffer* cmd) = 0;
    };

    [[nodiscard]] std::unique_ptr<RHI> createOpenGLRHI();

} // namespace Nox

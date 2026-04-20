// Copyright (c) 2026 NoxFly — AGPL-3.0

#include "OpenGLRHI.hpp"

#include <NoxEngine/core/Assert.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <stdexcept>
#include <string>

namespace Nox {

    // ════════════════════════════════════════════════════════════════
    //  OpenGLCommandBuffer
    // ════════════════════════════════════════════════════════════════

    OpenGLCommandBuffer::OpenGLCommandBuffer(
        HandlePool<GLBufferData>&   buffers,
        HandlePool<GLPipelineData>& pipelines
    )
        : buffers_(buffers)
        , pipelines_(pipelines) {}

    void OpenGLCommandBuffer::setViewport(int x, int y, int width, int height) {
        glViewport(x, y, width, height);
    }

    void OpenGLCommandBuffer::clear(float r, float g, float b, float a, float depth) {
        glClearColor(r, g, b, a);
        glClearDepth(static_cast<double>(depth));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLCommandBuffer::bindPipeline(PipelineHandle pipeline) {
        HandlePool<GLPipelineData>::Handle h{ pipeline.index, pipeline.generation };
        boundPipeline_ = pipelines_.get(h);
        if (boundPipeline_) {
            glUseProgram(boundPipeline_->program);
            // NOTE: DO NOT bind VAO here! VAO is per-mesh, not per-pipeline.
            // Each mesh will bind its own VAO before rendering.
        }
    }

    void OpenGLCommandBuffer::bindVertexBuffer(BufferHandle buffer) {
        HandlePool<GLBufferData>::Handle h{ buffer.index, buffer.generation };
        auto* buf = buffers_.get(h);
        if (buf) {
            glBindBuffer(GL_ARRAY_BUFFER, buf->id);

            // Position attribute (location 0): vec3
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                sizeof(float) * 8, reinterpret_cast<void*>(0));

            // Normal attribute (location 1): vec3
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                sizeof(float) * 8, reinterpret_cast<void*>(sizeof(float) * 3));

            // UV attribute (location 2): vec2
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                sizeof(float) * 8, reinterpret_cast<void*>(sizeof(float) * 6));
        }
    }

    void OpenGLCommandBuffer::bindIndexBuffer(BufferHandle buffer) {
        HandlePool<GLBufferData>::Handle h{ buffer.index, buffer.generation };
        auto* buf = buffers_.get(h);
        if (buf) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->id);
        }
    }

    void OpenGLCommandBuffer::pushConstant(std::string_view name, const Math::Mat4& value) {
        GLint loc = getUniformLocation(name);
        if (loc >= 0) {
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        }
    }

    void OpenGLCommandBuffer::pushConstant(std::string_view name, const Math::Vec3& value) {
        GLint loc = getUniformLocation(name);
        if (loc >= 0) {
            glUniform3fv(loc, 1, glm::value_ptr(value));
        }
    }

    void OpenGLCommandBuffer::pushConstant(std::string_view name, float value) {
        GLint loc = getUniformLocation(name);
        if (loc >= 0) {
            glUniform1f(loc, value);
        }
    }

    void OpenGLCommandBuffer::pushConstant(std::string_view name, int value) {
        GLint loc = getUniformLocation(name);
        if (loc >= 0) {
            glUniform1i(loc, value);
        }
    }

    void OpenGLCommandBuffer::drawIndexed(uint32_t indexCount) {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount),
                    GL_UNSIGNED_INT, nullptr);
    }

    void OpenGLCommandBuffer::drawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount) {
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(indexCount),
                                GL_UNSIGNED_INT, nullptr,
                                static_cast<GLsizei>(instanceCount));
    }

    GLint OpenGLCommandBuffer::getUniformLocation(std::string_view name) const {
        if (!boundPipeline_) return -1;
        std::string key(name);
        auto it = boundPipeline_->uniformLocations.find(key);
        if (it != boundPipeline_->uniformLocations.end()) {
            return it->second;
        }
        GLint loc = glGetUniformLocation(boundPipeline_->program, key.c_str());
        boundPipeline_->uniformLocations[key] = loc;
        return loc;
    }

    // ════════════════════════════════════════════════════════════════
    //  OpenGLRHI
    // ════════════════════════════════════════════════════════════════

    OpenGLRHI::OpenGLRHI() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        cmdBuffer_ = std::make_unique<OpenGLCommandBuffer>(buffers_, pipelines_);

        NOX_LOG_INFO("OpenGLRHI initialized");
    }

    OpenGLRHI::~OpenGLRHI() = default;

    BufferHandle OpenGLRHI::createBuffer(const BufferDesc& desc) {
        GLuint id = 0;
        glCreateBuffers(1, &id);

        glNamedBufferStorage(id, desc.sizeBytes, desc.data,
            desc.data ? 0 : GL_DYNAMIC_STORAGE_BIT);

        GLBufferData data{
            .id    = id,
            .usage = desc.usage,
            .size  = desc.sizeBytes
        };

        auto handle = buffers_.insert(std::move(data));
        return BufferHandle{ handle.index, handle.generation };
    }

    TextureHandle OpenGLRHI::createTexture(const TextureDesc& desc) {
        GLuint id = 0;
        glCreateTextures(GL_TEXTURE_2D, 1, &id);

        GLenum internalFormat = GL_RGBA8;
        GLenum format = GL_RGBA;
        switch (desc.format) {
            case TextureFormat::RGBA8:
                internalFormat = GL_RGBA8;
                format = GL_RGBA;
                break;
            case TextureFormat::RGB8:
                internalFormat = GL_RGB8;
                format = GL_RGB;
                break;
            case TextureFormat::Depth24Stencil8:
                internalFormat = GL_DEPTH24_STENCIL8;
                format = GL_DEPTH_STENCIL;
                break;
        }

        glTextureStorage2D(id, 1, internalFormat, desc.width, desc.height);

        if (desc.data && desc.format != TextureFormat::Depth24Stencil8) {
            glTextureSubImage2D(id, 0, 0, 0, desc.width, desc.height,
                                format, GL_UNSIGNED_BYTE, desc.data);
        }

        // Set default sampling parameters
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);

        GLTextureData data{
            .id     = id,
            .width  = desc.width,
            .height = desc.height,
            .format = desc.format
        };

        auto handle = textures_.insert(std::move(data));
        return TextureHandle{ handle.index, handle.generation };
    }

    ShaderHandle OpenGLRHI::createShader(const ShaderDesc& desc) {
        GLenum type = GL_VERTEX_SHADER;
        switch (desc.stage) {
            case ShaderStage::Vertex:   type = GL_VERTEX_SHADER;   break;
            case ShaderStage::Fragment: type = GL_FRAGMENT_SHADER; break;
            case ShaderStage::Compute:  type = GL_COMPUTE_SHADER;  break;
        }

        GLuint id = glCreateShader(type);
        const char* src = desc.source.data();
        GLint len = static_cast<GLint>(desc.source.size());
        glShaderSource(id, 1, &src, &len);
        glCompileShader(id);

        GLint success = 0;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[1024];
            glGetShaderInfoLog(id, sizeof(log), nullptr, log);
            glDeleteShader(id);
            throw std::runtime_error(std::string("Shader compilation failed: ") + log);
        }

        GLShaderData data{
            .id    = id,
            .stage = desc.stage
        };

        auto handle = shaders_.insert(std::move(data));
        return ShaderHandle{ handle.index, handle.generation };
    }

    PipelineHandle OpenGLRHI::createPipeline(const PipelineDesc& desc) {
        HandlePool<GLShaderData>::Handle vsH{ desc.vertexShader.index, desc.vertexShader.generation };
        HandlePool<GLShaderData>::Handle fsH{ desc.fragmentShader.index, desc.fragmentShader.generation };

        auto* vs = shaders_.get(vsH);
        auto* fs = shaders_.get(fsH);
        NOX_ASSERT_MSG(vs && fs, "Invalid shader handles for pipeline creation");

        GLuint program = glCreateProgram();
        glAttachShader(program, vs->id);
        glAttachShader(program, fs->id);
        glLinkProgram(program);

        GLint success = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            char log[1024];
            glGetProgramInfoLog(program, sizeof(log), nullptr, log);
            glDeleteProgram(program);
            throw std::runtime_error(std::string("Shader link failed: ") + log);
        }

        // Create a VAO for this pipeline
        GLuint vao = 0;
        glCreateVertexArrays(1, &vao);

        GLPipelineData data{
            .program = program,
            .vao     = vao,
            .uniformLocations = {}
        };

        auto handle = pipelines_.insert(std::move(data));
        return PipelineHandle{ handle.index, handle.generation };
    }

    void OpenGLRHI::destroyBuffer(BufferHandle handle) {
        HandlePool<GLBufferData>::Handle h{ handle.index, handle.generation };
        auto* buf = buffers_.get(h);
        if (buf) {
            glDeleteBuffers(1, &buf->id);
            buffers_.remove(h);
        }
    }

    void OpenGLRHI::destroyTexture(TextureHandle handle) {
        HandlePool<GLTextureData>::Handle h{ handle.index, handle.generation };
        auto* tex = textures_.get(h);
        if (tex) {
            glDeleteTextures(1, &tex->id);
            textures_.remove(h);
        }
    }

    void OpenGLRHI::destroyShader(ShaderHandle handle) {
        HandlePool<GLShaderData>::Handle h{ handle.index, handle.generation };
        auto* sh = shaders_.get(h);
        if (sh) {
            glDeleteShader(sh->id);
            shaders_.remove(h);
        }
    }

    void OpenGLRHI::destroyPipeline(PipelineHandle handle) {
        HandlePool<GLPipelineData>::Handle h{ handle.index, handle.generation };
        auto* pipe = pipelines_.get(h);
        if (pipe) {
            glDeleteProgram(pipe->program);
            glDeleteVertexArrays(1, &pipe->vao);
            pipelines_.remove(h);
        }
    }

    CommandBuffer* OpenGLRHI::beginFrame() {
        return cmdBuffer_.get();
    }

    void OpenGLRHI::endFrame([[maybe_unused]] CommandBuffer* cmd) {
        glBindVertexArray(0);
        glUseProgram(0);
    }

    // ── Factory ────────────────────────────────────────────────────
    std::unique_ptr<RHI> createOpenGLRHI() {
        return std::make_unique<OpenGLRHI>();
    }

} // namespace Nox

// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/PostProcessEffect.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

#include <array>
#include <stdexcept>
#include <string>

namespace Nox {

    uint32_t PostProcessEffect::compilePostProcessShader(std::string_view fragmentSource) {
        auto compileShader = [](GLenum type, std::string_view source) -> uint32_t {
            uint32_t shader = glCreateShader(type);
            const char* src = source.data();
            auto len = static_cast<GLint>(source.size());
            glShaderSource(shader, 1, &src, &len);
            glCompileShader(shader);

            GLint success = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (success == GL_FALSE) {
                GLint logLen = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
                std::string log(static_cast<std::string::size_type>(logLen), '\0');
                glGetShaderInfoLog(shader, logLen, nullptr, log.data());
                glDeleteShader(shader);
                throw std::runtime_error("PostProcess shader compile error: " + log);
            }

            return shader;
        };

        uint32_t vs = compileShader(GL_VERTEX_SHADER, QuadVertexSource);
        uint32_t fs = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

        uint32_t program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        GLint success = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (success == GL_FALSE) {
            GLint logLen = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
            std::string log(static_cast<std::string::size_type>(logLen), '\0');
            glGetProgramInfoLog(program, logLen, nullptr, log.data());
            glDeleteShader(vs);
            glDeleteShader(fs);
            glDeleteProgram(program);
            throw std::runtime_error("PostProcess program link error: " + log);
        }

        glDetachShader(program, vs);
        glDetachShader(program, fs);
        glDeleteShader(vs);
        glDeleteShader(fs);

        return program;
    }

    void PostProcessEffect::drawFullscreenQuad() {
        static uint32_t quadVAO = 0;
        static uint32_t quadVBO = 0;

        if (quadVAO == 0) {
            std::array<float, 16> vertices = {
                // pos        uv
                -1.0f, -1.0f,  0.0f, 0.0f,
                 1.0f, -1.0f,  1.0f, 0.0f,
                -1.0f,  1.0f,  0.0f, 1.0f,
                 1.0f,  1.0f,  1.0f, 1.0f,
            };

            glCreateVertexArrays(1, &quadVAO);
            glCreateBuffers(1, &quadVBO);
            glNamedBufferStorage(quadVBO, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
                                 vertices.data(), 0);

            glEnableVertexArrayAttrib(quadVAO, 0);
            glVertexArrayAttribFormat(quadVAO, 0, 2, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(quadVAO, 0, 0);

            glEnableVertexArrayAttrib(quadVAO, 1);
            glVertexArrayAttribFormat(quadVAO, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float));
            glVertexArrayAttribBinding(quadVAO, 1, 0);

            glVertexArrayVertexBuffer(quadVAO, 0, quadVBO, 0, 4 * sizeof(float));
        }

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

} // namespace Nox

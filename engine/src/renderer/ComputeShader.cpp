// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/ComputeShader.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>

#include <utility>

namespace Nox {

    ComputeShader::~ComputeShader() {
        if (program_ != 0) {
            glDeleteProgram(program_);
        }
    }

    ComputeShader::ComputeShader(ComputeShader&& other) noexcept
        : program_(std::exchange(other.program_, 0))
        , uniformCache_(std::move(other.uniformCache_))
    {}

    ComputeShader& ComputeShader::operator=(ComputeShader&& other) noexcept {
        if (this != &other) {
            if (program_ != 0) {
                glDeleteProgram(program_);
            }
            program_ = std::exchange(other.program_, 0);
            uniformCache_ = std::move(other.uniformCache_);
        }
        return *this;
    }

    bool ComputeShader::compile(std::string_view source) {
        GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
        const char* src = source.data();
        auto len = static_cast<GLint>(source.size());
        glShaderSource(shader, 1, &src, &len);
        glCompileShader(shader);

        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            NOX_LOG_ERROR("Compute shader compilation failed: {}", infoLog);
            glDeleteShader(shader);
            return false;
        }

        GLuint prog = glCreateProgram();
        glAttachShader(prog, shader);
        glLinkProgram(prog);
        glDeleteShader(shader);

        glGetProgramiv(prog, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(prog, sizeof(infoLog), nullptr, infoLog);
            NOX_LOG_ERROR("Compute shader link failed: {}", infoLog);
            glDeleteProgram(prog);
            return false;
        }

        if (program_ != 0) {
            glDeleteProgram(program_);
        }

        program_ = prog;
        uniformCache_.clear();
        return true;
    }

    void ComputeShader::dispatch(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ) const {
        if (program_ == 0) { return; }
        glUseProgram(program_);
        glDispatchCompute(groupsX, groupsY, groupsZ);
    }

    void ComputeShader::barrier(uint32_t barriers) {
        glMemoryBarrier(barriers);
    }

    void ComputeShader::bind() const {
        glUseProgram(program_);
    }

    void ComputeShader::unbind() {
        glUseProgram(0);
    }

    int ComputeShader::getUniformLocation(std::string_view name) {
        auto key = std::string(name);
        auto it = uniformCache_.find(key);
        if (it != uniformCache_.end()) {
            return it->second;
        }
        int loc = glGetUniformLocation(program_, key.c_str());
        uniformCache_[key] = loc;
        return loc;
    }

    void ComputeShader::setUniform(std::string_view name, int value) {
        glProgramUniform1i(program_, getUniformLocation(name), value);
    }

    void ComputeShader::setUniform(std::string_view name, float value) {
        glProgramUniform1f(program_, getUniformLocation(name), value);
    }

    void ComputeShader::setUniform(std::string_view name, const Math::Vec2& value) {
        glProgramUniform2fv(program_, getUniformLocation(name), 1, glm::value_ptr(value));
    }

    void ComputeShader::setUniform(std::string_view name, const Math::Vec3& value) {
        glProgramUniform3fv(program_, getUniformLocation(name), 1, glm::value_ptr(value));
    }

    void ComputeShader::setUniform(std::string_view name, const Math::Vec4& value) {
        glProgramUniform4fv(program_, getUniformLocation(name), 1, glm::value_ptr(value));
    }

    void ComputeShader::setUniform(std::string_view name, const Math::Mat4& value) {
        glProgramUniformMatrix4fv(program_, getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void ComputeShader::bindSSBO(uint32_t buffer, uint32_t bindingPoint) {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, buffer);
    }

} // namespace Nox

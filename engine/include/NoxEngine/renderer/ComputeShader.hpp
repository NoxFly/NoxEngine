// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Nox {

    /// Wraps a compiled OpenGL compute shader program.
    /// Provides methods to dispatch work groups and bind uniforms / SSBOs.
    class ComputeShader {
    public:
        ComputeShader() = default;
        ~ComputeShader();

        ComputeShader(const ComputeShader&) = delete;
        ComputeShader& operator=(const ComputeShader&) = delete;
        ComputeShader(ComputeShader&&) noexcept;
        ComputeShader& operator=(ComputeShader&&) noexcept;

        /// Compile from GLSL source.
        /// @return true on success.
        [[nodiscard]] bool compile(std::string_view source);

        /// Dispatch the compute shader.
        void dispatch(uint32_t groupsX, uint32_t groupsY = 1, uint32_t groupsZ = 1) const;

        /// Insert a memory barrier (e.g. after writing to an SSBO).
        static void barrier(uint32_t barriers);

        /// Bind the shader program.
        void bind() const;

        /// Unbind any compute program.
        static void unbind();

        // ── Uniforms ───────────────────────────────────────────────
        void setUniform(std::string_view name, int value);
        void setUniform(std::string_view name, float value);
        void setUniform(std::string_view name, const Math::Vec2& value);
        void setUniform(std::string_view name, const Math::Vec3& value);
        void setUniform(std::string_view name, const Math::Vec4& value);
        void setUniform(std::string_view name, const Math::Mat4& value);

        /// Bind a buffer to a shader storage binding point.
        static void bindSSBO(uint32_t buffer, uint32_t bindingPoint);

        [[nodiscard]] bool isCompiled() const { return program_ != 0; }
        [[nodiscard]] uint32_t program() const { return program_; }

    private:
        [[nodiscard]] int getUniformLocation(std::string_view name);

        uint32_t program_ = 0;
        std::unordered_map<std::string, int> uniformCache_;
    };

} // namespace Nox

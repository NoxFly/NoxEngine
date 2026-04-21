// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/physics/PhysicsDebugRenderer.hpp>
#include <NoxEngine/physics/PhysicsWorld.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>

namespace Nox {

    static constexpr std::string_view DebugLineVertexSource = R"glsl(
    #version 460 core
    layout(location = 0) in vec3 aPosition;
    layout(location = 1) in vec3 aColor;
    uniform mat4 uMVP;
    out vec3 vColor;
    void main() {
        vColor = aColor;
        gl_Position = uMVP * vec4(aPosition, 1.0);
    }
    )glsl";

    static constexpr std::string_view DebugLineFragmentSource = R"glsl(
    #version 460 core
    in vec3 vColor;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(vColor, 0.7);
    }
    )glsl";

    PhysicsDebugRenderer::PhysicsDebugRenderer() = default;

    PhysicsDebugRenderer::~PhysicsDebugRenderer() {
        if (initialized_) {
            glDeleteVertexArrays(1, &vao_);
            glDeleteBuffers(1, &vbo_);
            if (program_ != 0) {
                glDeleteProgram(program_);
            }
        }
    }

    void PhysicsDebugRenderer::init() {
        if (initialized_) { return; }

        auto compileShader = [](GLenum type, std::string_view src) -> GLuint {
            GLuint shader = glCreateShader(type);
            const char* srcPtr = src.data();
            auto len = static_cast<GLint>(src.size());
            glShaderSource(shader, 1, &srcPtr, &len);
            glCompileShader(shader);
            GLint success = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                char log[512]{};
                glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
                NOX_LOG_ERROR("Physics debug shader error: {}", log);
                glDeleteShader(shader);
                return 0;
            }
            return shader;
        };

        GLuint vs = compileShader(GL_VERTEX_SHADER, DebugLineVertexSource);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, DebugLineFragmentSource);

        if (vs == 0 || fs == 0) {
            if (vs) { glDeleteShader(vs); }
            if (fs) { glDeleteShader(fs); }
            return;
        }

        program_ = glCreateProgram();
        glAttachShader(program_, vs);
        glAttachShader(program_, fs);
        glLinkProgram(program_);
        glDeleteShader(vs);
        glDeleteShader(fs);

        glCreateVertexArrays(1, &vao_);
        glCreateBuffers(1, &vbo_);

        // Pre-allocate a large enough buffer for debug lines
        glNamedBufferStorage(vbo_, sizeof(DebugLine) * 8192,
                             nullptr, GL_DYNAMIC_STORAGE_BIT);

        glVertexArrayVertexBuffer(vao_, 0, vbo_, 0, sizeof(Math::Vec3) * 2);

        glEnableVertexArrayAttrib(vao_, 0);
        glVertexArrayAttribFormat(vao_, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(vao_, 0, 0);

        glEnableVertexArrayAttrib(vao_, 1);
        glVertexArrayAttribFormat(vao_, 1, 3, GL_FLOAT, GL_FALSE, sizeof(Math::Vec3));
        glVertexArrayAttribBinding(vao_, 1, 0);

        initialized_ = true;
    }

    void PhysicsDebugRenderer::update([[maybe_unused]] const PhysicsWorld& world) {
        if (!enabled_) { return; }

        lines_.clear();

        // Note: A full implementation would query Jolt's DebugRenderer interface
        // to get all body shapes. For now we provide the infrastructure,
        // and shapes can be added manually via addBox/addSphere/addCapsule.
    }

    void PhysicsDebugRenderer::render(const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix) {
        if (!enabled_ || !initialized_ || lines_.empty()) { return; }

        auto dataSize = static_cast<GLsizeiptr>(lines_.size() * sizeof(DebugLine));
        glNamedBufferSubData(vbo_, 0, dataSize, lines_.data());

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth(1.5f);

        glUseProgram(program_);
        Math::Mat4 mvp = projMatrix * viewMatrix;
        glUniformMatrix4fv(glGetUniformLocation(program_, "uMVP"),
                           1, GL_FALSE, glm::value_ptr(mvp));

        glBindVertexArray(vao_);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lines_.size() * 2));

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glLineWidth(1.0f);
    }

    void PhysicsDebugRenderer::addBox(const Math::Vec3& center, const Math::Vec3& halfExtents,
                                       const Math::Quat& rotation, const Math::Vec3& color) {
        Math::Mat4 rotMat = glm::mat4_cast(rotation);

        Math::Vec3 corners[8];
        for (int i = 0; i < 8; ++i) {
            Math::Vec3 local{
                (i & 1) ? halfExtents.x : -halfExtents.x,
                (i & 2) ? halfExtents.y : -halfExtents.y,
                (i & 4) ? halfExtents.z : -halfExtents.z
            };
            corners[i] = center + Math::Vec3(rotMat * Math::Vec4(local, 0.0f));
        }

        int edges[12][2] = {
            {0,1},{2,3},{4,5},{6,7},
            {0,2},{1,3},{4,6},{5,7},
            {0,4},{1,5},{2,6},{3,7}
        };

        for (auto& e : edges) {
            lines_.push_back({ corners[e[0]], color, corners[e[1]], color });
        }
    }

    void PhysicsDebugRenderer::addSphere(const Math::Vec3& center, float radius,
                                          const Math::Vec3& color) {
        constexpr int Segments = 24;
        for (int i = 0; i < Segments; ++i) {
            float a0 = static_cast<float>(i) / static_cast<float>(Segments) * 2.0f * glm::pi<float>();
            float a1 = static_cast<float>(i + 1) / static_cast<float>(Segments) * 2.0f * glm::pi<float>();

            // XY circle
            lines_.push_back({
                center + Math::Vec3(std::cos(a0), std::sin(a0), 0) * radius, color,
                center + Math::Vec3(std::cos(a1), std::sin(a1), 0) * radius, color
            });
            // XZ circle
            lines_.push_back({
                center + Math::Vec3(std::cos(a0), 0, std::sin(a0)) * radius, color,
                center + Math::Vec3(std::cos(a1), 0, std::sin(a1)) * radius, color
            });
            // YZ circle
            lines_.push_back({
                center + Math::Vec3(0, std::cos(a0), std::sin(a0)) * radius, color,
                center + Math::Vec3(0, std::cos(a1), std::sin(a1)) * radius, color
            });
        }
    }

    void PhysicsDebugRenderer::addCapsule(const Math::Vec3& center, float radius,
                                           float halfHeight,
                                           const Math::Quat& rotation,
                                           const Math::Vec3& color) {
        Math::Mat4 rotMat = glm::mat4_cast(rotation);

        Math::Vec3 top = center + Math::Vec3(rotMat * Math::Vec4(0, halfHeight, 0, 0));
        Math::Vec3 bottom = center + Math::Vec3(rotMat * Math::Vec4(0, -halfHeight, 0, 0));

        // Top and bottom sphere approximations
        addSphere(top, radius, color);
        addSphere(bottom, radius, color);

        // Connecting lines
        constexpr int Segments = 8;
        for (int i = 0; i < Segments; ++i) {
            float angle = static_cast<float>(i) / static_cast<float>(Segments) * 2.0f * glm::pi<float>();
            Math::Vec3 offset = Math::Vec3(rotMat * Math::Vec4(
                std::cos(angle) * radius, 0, std::sin(angle) * radius, 0));
            lines_.push_back({ top + offset, color, bottom + offset, color });
        }
    }

} // namespace Nox

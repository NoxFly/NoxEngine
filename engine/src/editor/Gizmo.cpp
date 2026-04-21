// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/editor/Gizmo.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <cmath>
#include <vector>

namespace Nox {

    // ── Simple line shader ─────────────────────────────────────────

    static constexpr std::string_view GizmoVertexSource = R"glsl(
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

    static constexpr std::string_view GizmoFragmentSource = R"glsl(
    #version 460 core
    in vec3 vColor;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(vColor, 1.0);
    }
    )glsl";

    // ── Vertex with color ──────────────────────────────────────────

    struct GizmoVertex {
        Math::Vec3 position;
        Math::Vec3 color;
    };

    // ── Gizmo Implementation ──────────────────────────────────────

    Gizmo::Gizmo() = default;

    Gizmo::~Gizmo() {
        if (gpuReady_) {
            glDeleteVertexArrays(1, &vao_);
            glDeleteBuffers(1, &vbo_);
            if (pipeline_ != 0) {
                glDeleteProgram(pipeline_);
            }
        }
    }

    void Gizmo::initGpuResources() {
        if (gpuReady_) {
            return;
        }

        // Compile gizmo shader
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
                NOX_LOG_ERROR("Gizmo shader compile error: {}", log);
                glDeleteShader(shader);
                return 0;
            }
            return shader;
        };

        GLuint vs = compileShader(GL_VERTEX_SHADER, GizmoVertexSource);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, GizmoFragmentSource);

        if (vs == 0 || fs == 0) {
            if (vs) { glDeleteShader(vs); }
            if (fs) { glDeleteShader(fs); }
            return;
        }

        pipeline_ = glCreateProgram();
        glAttachShader(pipeline_, vs);
        glAttachShader(pipeline_, fs);
        glLinkProgram(pipeline_);

        GLint linked = 0;
        glGetProgramiv(pipeline_, GL_LINK_STATUS, &linked);
        if (!linked) {
            char log[512]{};
            glGetProgramInfoLog(pipeline_, sizeof(log), nullptr, log);
            NOX_LOG_ERROR("Gizmo shader link error: {}", log);
            glDeleteProgram(pipeline_);
            pipeline_ = 0;
        }

        glDeleteShader(vs);
        glDeleteShader(fs);

        // Create VAO and VBO for dynamic line drawing
        glCreateVertexArrays(1, &vao_);
        glCreateBuffers(1, &vbo_);

        // Allocate enough space for gizmo geometry (generous max)
        glNamedBufferStorage(vbo_, sizeof(GizmoVertex) * 512,
                             nullptr, GL_DYNAMIC_STORAGE_BIT);

        glVertexArrayVertexBuffer(vao_, 0, vbo_, 0, sizeof(GizmoVertex));

        // Position
        glEnableVertexArrayAttrib(vao_, 0);
        glVertexArrayAttribFormat(vao_, 0, 3, GL_FLOAT, GL_FALSE, offsetof(GizmoVertex, position));
        glVertexArrayAttribBinding(vao_, 0, 0);

        // Color
        glEnableVertexArrayAttrib(vao_, 1);
        glVertexArrayAttribFormat(vao_, 1, 3, GL_FLOAT, GL_FALSE, offsetof(GizmoVertex, color));
        glVertexArrayAttribBinding(vao_, 1, 0);

        gpuReady_ = true;
    }

    Math::Vec2 Gizmo::worldToScreen(const Math::Vec3& worldPos,
                                     const Math::Mat4& mvp,
                                     int width, int height) const {
        Math::Vec4 clip = mvp * Math::Vec4(worldPos, 1.0f);
        if (clip.w <= 0.0f) {
            return { -1.0f, -1.0f };
        }
        Math::Vec3 ndc = Math::Vec3(clip) / clip.w;
        return {
            (ndc.x * 0.5f + 0.5f) * static_cast<float>(width),
            (1.0f - (ndc.y * 0.5f + 0.5f)) * static_cast<float>(height)
        };
    }

    bool Gizmo::update(const Input& input, const PerspectiveCamera& camera,
                        int viewportWidth, int viewportHeight) {
        if (!target_) {
            return false;
        }

        Math::Mat4 vp = camera.projectionMatrix() * camera.viewMatrix();
        Math::Vec3 objPos = target_->transform().position();
        float gizmoSize = 1.5f;

        Math::Vec2 mousePos = input.mousePosition();

        // Test axis proximity
        auto testAxis = [&](const Math::Vec3& axisEnd) -> float {
            Math::Vec2 screenStart = worldToScreen(objPos, vp, viewportWidth, viewportHeight);
            Math::Vec2 screenEnd = worldToScreen(axisEnd, vp, viewportWidth, viewportHeight);

            Math::Vec2 lineDir = screenEnd - screenStart;
            float lineLen = glm::length(lineDir);
            if (lineLen < 1.0f) { return 999.0f; }
            lineDir /= lineLen;

            Math::Vec2 toMouse = mousePos - screenStart;
            float t = glm::dot(toMouse, lineDir);
            t = glm::clamp(t, 0.0f, lineLen);

            Math::Vec2 closest = screenStart + lineDir * t;
            return glm::length(mousePos - closest);
        };

        // Determine hovered axis
        if (!dragging_) {
            float distX = testAxis(objPos + Math::Vec3(gizmoSize, 0, 0));
            float distY = testAxis(objPos + Math::Vec3(0, gizmoSize, 0));
            float distZ = testAxis(objPos + Math::Vec3(0, 0, gizmoSize));

            constexpr float HitThreshold = 15.0f;
            float minDist = std::min({ distX, distY, distZ });

            if (minDist > HitThreshold) {
                activeAxis_ = Axis::None;
            }
            else if (minDist == distX) {
                activeAxis_ = Axis::X;
            }
            else if (minDist == distY) {
                activeAxis_ = Axis::Y;
            }
            else {
                activeAxis_ = Axis::Z;
            }
        }

        // Start drag
        if (activeAxis_ != Axis::None && input.isMouseButtonPressed(MouseButton::Left)) {
            dragging_ = true;
            dragStart_ = mousePos;
            dragStartPos_ = target_->transform().position();
            dragStartScale_ = target_->transform().scale();
            dragStartRot_ = target_->transform().rotation();
            return true;
        }

        // During drag
        if (dragging_ && input.isMouseButtonDown(MouseButton::Left)) {
            Math::Vec2 delta = mousePos - dragStart_;

            // Get axis direction in screen space
            Math::Vec3 axisDir{ 0.0f };
            switch (activeAxis_) {
                case Axis::X: axisDir = { 1.0f, 0.0f, 0.0f }; break;
                case Axis::Y: axisDir = { 0.0f, 1.0f, 0.0f }; break;
                case Axis::Z: axisDir = { 0.0f, 0.0f, 1.0f }; break;
                default: break;
            }

            Math::Vec2 screenAxis = worldToScreen(objPos + axisDir, vp, viewportWidth, viewportHeight)
                                  - worldToScreen(objPos, vp, viewportWidth, viewportHeight);
            float screenAxisLen = glm::length(screenAxis);
            if (screenAxisLen < 0.001f) { return true; }

            float projection = glm::dot(delta, screenAxis / screenAxisLen);
            float sensitivity = 0.01f;

            switch (mode_) {
                case Mode::Translate: {
                    Math::Vec3 newPos = dragStartPos_ + axisDir * projection * sensitivity;
                    target_->setPosition(newPos.x, newPos.y, newPos.z);
                    break;
                }
                case Mode::Rotate: {
                    float angle = projection * 0.5f;
                    Math::Quat rot = glm::angleAxis(glm::radians(angle), axisDir);
                    target_->transform().setRotation(rot * dragStartRot_);
                    break;
                }
                case Mode::Scale: {
                    float scaleFactor = 1.0f + projection * sensitivity;
                    scaleFactor = glm::clamp(scaleFactor, 0.01f, 100.0f);
                    Math::Vec3 newScale = dragStartScale_;
                    switch (activeAxis_) {
                        case Axis::X: newScale.x *= scaleFactor; break;
                        case Axis::Y: newScale.y *= scaleFactor; break;
                        case Axis::Z: newScale.z *= scaleFactor; break;
                        default: break;
                    }
                    target_->transform().setScale(newScale);
                    break;
                }
            }

            return true;
        }

        // End drag
        if (dragging_ && !input.isMouseButtonDown(MouseButton::Left)) {
            dragging_ = false;
        }

        return activeAxis_ != Axis::None;
    }

    void Gizmo::render(const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix,
                        int viewportWidth, int viewportHeight) {
        if (!target_) {
            return;
        }

        initGpuResources();
        if (!gpuReady_ || pipeline_ == 0) {
            return;
        }

        Math::Mat4 mvp = projMatrix * viewMatrix;
        Math::Vec3 center = target_->transform().position();

        switch (mode_) {
            case Mode::Translate:
                renderTranslateGizmo(mvp, center);
                break;
            case Mode::Rotate:
                renderRotateGizmo(mvp, center);
                break;
            case Mode::Scale:
                renderScaleGizmo(mvp, center);
                break;
        }

        // Ignore unused params - used by update() for screen-space projection
        static_cast<void>(viewportWidth);
        static_cast<void>(viewportHeight);
    }

    void Gizmo::renderTranslateGizmo(const Math::Mat4& mvp, const Math::Vec3& center) {
        constexpr float AxisLen = 1.5f;
        constexpr float ArrowLen = 0.2f;
        constexpr float ArrowWidth = 0.06f;

        Math::Vec3 xColor = (activeAxis_ == Axis::X) ? Math::Vec3(1.0f, 1.0f, 0.0f) : Math::Vec3(1.0f, 0.2f, 0.2f);
        Math::Vec3 yColor = (activeAxis_ == Axis::Y) ? Math::Vec3(1.0f, 1.0f, 0.0f) : Math::Vec3(0.2f, 1.0f, 0.2f);
        Math::Vec3 zColor = (activeAxis_ == Axis::Z) ? Math::Vec3(1.0f, 1.0f, 0.0f) : Math::Vec3(0.2f, 0.2f, 1.0f);

        std::vector<GizmoVertex> verts;

        // X axis line + arrow
        verts.push_back({ center, xColor });
        verts.push_back({ center + Math::Vec3(AxisLen, 0, 0), xColor });
        Math::Vec3 xTip = center + Math::Vec3(AxisLen, 0, 0);
        verts.push_back({ xTip, xColor });
        verts.push_back({ xTip - Math::Vec3(ArrowLen, ArrowWidth, 0), xColor });
        verts.push_back({ xTip, xColor });
        verts.push_back({ xTip - Math::Vec3(ArrowLen, -ArrowWidth, 0), xColor });

        // Y axis line + arrow
        verts.push_back({ center, yColor });
        verts.push_back({ center + Math::Vec3(0, AxisLen, 0), yColor });
        Math::Vec3 yTip = center + Math::Vec3(0, AxisLen, 0);
        verts.push_back({ yTip, yColor });
        verts.push_back({ yTip - Math::Vec3(ArrowWidth, ArrowLen, 0), yColor });
        verts.push_back({ yTip, yColor });
        verts.push_back({ yTip - Math::Vec3(-ArrowWidth, ArrowLen, 0), yColor });

        // Z axis line + arrow
        verts.push_back({ center, zColor });
        verts.push_back({ center + Math::Vec3(0, 0, AxisLen), zColor });
        Math::Vec3 zTip = center + Math::Vec3(0, 0, AxisLen);
        verts.push_back({ zTip, zColor });
        verts.push_back({ zTip - Math::Vec3(0, ArrowWidth, ArrowLen), zColor });
        verts.push_back({ zTip, zColor });
        verts.push_back({ zTip - Math::Vec3(0, -ArrowWidth, ArrowLen), zColor });

        // Upload and draw
        glNamedBufferSubData(vbo_, 0,
                             static_cast<GLsizeiptr>(verts.size() * sizeof(GizmoVertex)),
                             verts.data());

        glDisable(GL_DEPTH_TEST);
        glLineWidth(2.5f);
        glUseProgram(pipeline_);

        GLint mvpLoc = glGetUniformLocation(pipeline_, "uMVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        glBindVertexArray(vao_);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(verts.size()));

        glEnable(GL_DEPTH_TEST);
        glLineWidth(1.0f);
    }

    void Gizmo::renderRotateGizmo(const Math::Mat4& mvp, const Math::Vec3& center) {
        constexpr float Radius = 1.2f;
        constexpr int Segments = 64;

        Math::Vec3 xColor = (activeAxis_ == Axis::X) ? Math::Vec3(1.0f, 1.0f, 0.0f) : Math::Vec3(1.0f, 0.2f, 0.2f);
        Math::Vec3 yColor = (activeAxis_ == Axis::Y) ? Math::Vec3(1.0f, 1.0f, 0.0f) : Math::Vec3(0.2f, 1.0f, 0.2f);
        Math::Vec3 zColor = (activeAxis_ == Axis::Z) ? Math::Vec3(1.0f, 1.0f, 0.0f) : Math::Vec3(0.2f, 0.2f, 1.0f);

        std::vector<GizmoVertex> verts;

        // X rotation ring (YZ plane)
        for (int i = 0; i < Segments; ++i) {
            float a0 = static_cast<float>(i) / static_cast<float>(Segments) * 2.0f * glm::pi<float>();
            float a1 = static_cast<float>(i + 1) / static_cast<float>(Segments) * 2.0f * glm::pi<float>();
            verts.push_back({ center + Math::Vec3(0, std::cos(a0), std::sin(a0)) * Radius, xColor });
            verts.push_back({ center + Math::Vec3(0, std::cos(a1), std::sin(a1)) * Radius, xColor });
        }

        // Y rotation ring (XZ plane)
        for (int i = 0; i < Segments; ++i) {
            float a0 = static_cast<float>(i) / static_cast<float>(Segments) * 2.0f * glm::pi<float>();
            float a1 = static_cast<float>(i + 1) / static_cast<float>(Segments) * 2.0f * glm::pi<float>();
            verts.push_back({ center + Math::Vec3(std::cos(a0), 0, std::sin(a0)) * Radius, yColor });
            verts.push_back({ center + Math::Vec3(std::cos(a1), 0, std::sin(a1)) * Radius, yColor });
        }

        // Z rotation ring (XY plane)
        for (int i = 0; i < Segments; ++i) {
            float a0 = static_cast<float>(i) / static_cast<float>(Segments) * 2.0f * glm::pi<float>();
            float a1 = static_cast<float>(i + 1) / static_cast<float>(Segments) * 2.0f * glm::pi<float>();
            verts.push_back({ center + Math::Vec3(std::cos(a0), std::sin(a0), 0) * Radius, zColor });
            verts.push_back({ center + Math::Vec3(std::cos(a1), std::sin(a1), 0) * Radius, zColor });
        }

        glNamedBufferSubData(vbo_, 0,
                             static_cast<GLsizeiptr>(verts.size() * sizeof(GizmoVertex)),
                             verts.data());

        glDisable(GL_DEPTH_TEST);
        glLineWidth(2.0f);
        glUseProgram(pipeline_);

        GLint mvpLoc = glGetUniformLocation(pipeline_, "uMVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        glBindVertexArray(vao_);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(verts.size()));

        glEnable(GL_DEPTH_TEST);
        glLineWidth(1.0f);
    }

    void Gizmo::renderScaleGizmo(const Math::Mat4& mvp, const Math::Vec3& center) {
        constexpr float AxisLen = 1.5f;
        constexpr float CubeSize = 0.08f;

        Math::Vec3 xColor = (activeAxis_ == Axis::X) ? Math::Vec3(1.0f, 1.0f, 0.0f) : Math::Vec3(1.0f, 0.2f, 0.2f);
        Math::Vec3 yColor = (activeAxis_ == Axis::Y) ? Math::Vec3(1.0f, 1.0f, 0.0f) : Math::Vec3(0.2f, 1.0f, 0.2f);
        Math::Vec3 zColor = (activeAxis_ == Axis::Z) ? Math::Vec3(1.0f, 1.0f, 0.0f) : Math::Vec3(0.2f, 0.2f, 1.0f);

        std::vector<GizmoVertex> verts;

        auto addCube = [&](const Math::Vec3& pos, const Math::Vec3& color) {
            float s = CubeSize;
            // 12 edges of a cube as lines (24 vertices)
            Math::Vec3 corners[8] = {
                pos + Math::Vec3(-s, -s, -s), pos + Math::Vec3( s, -s, -s),
                pos + Math::Vec3( s,  s, -s), pos + Math::Vec3(-s,  s, -s),
                pos + Math::Vec3(-s, -s,  s), pos + Math::Vec3( s, -s,  s),
                pos + Math::Vec3( s,  s,  s), pos + Math::Vec3(-s,  s,  s),
            };
            int edges[12][2] = {
                {0,1},{1,2},{2,3},{3,0},
                {4,5},{5,6},{6,7},{7,4},
                {0,4},{1,5},{2,6},{3,7}
            };
            for (auto& e : edges) {
                verts.push_back({ corners[e[0]], color });
                verts.push_back({ corners[e[1]], color });
            }
        };

        // X axis line + cube
        verts.push_back({ center, xColor });
        verts.push_back({ center + Math::Vec3(AxisLen, 0, 0), xColor });
        addCube(center + Math::Vec3(AxisLen, 0, 0), xColor);

        // Y axis line + cube
        verts.push_back({ center, yColor });
        verts.push_back({ center + Math::Vec3(0, AxisLen, 0), yColor });
        addCube(center + Math::Vec3(0, AxisLen, 0), yColor);

        // Z axis line + cube
        verts.push_back({ center, zColor });
        verts.push_back({ center + Math::Vec3(0, 0, AxisLen), zColor });
        addCube(center + Math::Vec3(0, 0, AxisLen), zColor);

        glNamedBufferSubData(vbo_, 0,
                             static_cast<GLsizeiptr>(verts.size() * sizeof(GizmoVertex)),
                             verts.data());

        glDisable(GL_DEPTH_TEST);
        glLineWidth(2.5f);
        glUseProgram(pipeline_);

        GLint mvpLoc = glGetUniformLocation(pipeline_, "uMVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        glBindVertexArray(vao_);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(verts.size()));

        glEnable(GL_DEPTH_TEST);
        glLineWidth(1.0f);
    }

} // namespace Nox

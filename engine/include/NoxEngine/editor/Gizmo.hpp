// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>
#include <NoxEngine/platform/Input.hpp>
#include <NoxEngine/renderer/Camera.hpp>
#include <NoxEngine/scene/SceneObject.hpp>

#include <cstdint>

namespace Nox {

    /// 3D transform gizmo rendered as an overlay on the viewport.
    /// Supports translate, rotate, and scale manipulation modes.
    class Gizmo {
    public:
        enum class Mode { Translate, Rotate, Scale };
        enum class Axis { None, X, Y, Z };

        Gizmo();
        ~Gizmo();

        Gizmo(const Gizmo&) = delete;
        Gizmo& operator=(const Gizmo&) = delete;
        Gizmo(Gizmo&&) = default;
        Gizmo& operator=(Gizmo&&) = default;

        /// Set the current manipulation mode.
        void setMode(Mode mode) { mode_ = mode; }
        [[nodiscard]] Mode mode() const { return mode_; }

        /// Set the object to manipulate.
        void setTarget(SceneObject* target) { target_ = target; }
        [[nodiscard]] SceneObject* target() const { return target_; }

        /// Update gizmo interaction. Returns true if the gizmo consumed the input.
        bool update(const Input& input, const PerspectiveCamera& camera,
                    int viewportWidth, int viewportHeight);

        /// Render the gizmo overlay. Call after the main scene render, before ImGui.
        void render(const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix,
                    int viewportWidth, int viewportHeight);

    private:
        void initGpuResources();
        void renderTranslateGizmo(const Math::Mat4& mvp, const Math::Vec3& center);
        void renderRotateGizmo(const Math::Mat4& mvp, const Math::Vec3& center);
        void renderScaleGizmo(const Math::Mat4& mvp, const Math::Vec3& center);

        [[nodiscard]] Math::Vec2 worldToScreen(const Math::Vec3& worldPos,
                                                const Math::Mat4& mvp,
                                                int width, int height) const;

        Mode         mode_   = Mode::Translate;
        Axis         activeAxis_ = Axis::None;
        SceneObject* target_ = nullptr;
        bool         dragging_ = false;
        Math::Vec2   dragStart_{ 0.0f };
        Math::Vec3   dragStartPos_{ 0.0f };
        Math::Vec3   dragStartScale_{ 1.0f };
        Math::Quat   dragStartRot_{ 1.0f, 0.0f, 0.0f, 0.0f };

        // GPU resources
        uint32_t pipeline_   = 0;
        uint32_t vao_        = 0;
        uint32_t vbo_        = 0;
        bool     gpuReady_   = false;
    };

} // namespace Nox

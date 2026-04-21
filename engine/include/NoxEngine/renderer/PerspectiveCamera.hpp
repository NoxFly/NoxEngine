// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

namespace Nox {

    /// Perspective projection camera with position/target, lazy matrix updates.
    class PerspectiveCamera {
    public:
        PerspectiveCamera(float fovDegrees, float aspect, float nearPlane, float farPlane);

        void setPosition(float x, float y, float z);
        void setPosition(const Math::Vec3& pos);
        void lookAt(float x, float y, float z);
        void lookAt(const Math::Vec3& target);

        void setAspect(float aspect);
        void setFov(float fovDegrees);

        [[nodiscard]] const Math::Mat4& viewMatrix() const;
        [[nodiscard]] const Math::Mat4& projectionMatrix() const;
        [[nodiscard]] const Math::Vec3& position() const { return position_; }
        [[nodiscard]] const Math::Vec3& target() const { return target_; }
        [[nodiscard]] float near() const { return near_; }
        [[nodiscard]] float far() const { return far_; }
        [[nodiscard]] float fovDegrees() const { return glm::degrees(fovRadians_); }
        [[nodiscard]] float aspect() const { return aspect_; }

    private:
        void updateView() const;
        void updateProjection() const;

        Math::Vec3 position_{ 0.0f, 0.0f, 0.0f };
        Math::Vec3 target_{ 0.0f, 0.0f, 0.0f };
        Math::Vec3 up_{ 0.0f, 1.0f, 0.0f };

        float fovRadians_ = 0.0f;
        float aspect_     = 1.0f;
        float near_       = 0.1f;
        float far_        = 1000.0f;

        mutable Math::Mat4 viewMatrix_{ 1.0f };
        mutable Math::Mat4 projMatrix_{ 1.0f };
        mutable bool viewDirty_ = true;
        mutable bool projDirty_ = true;
    };

} // namespace Nox

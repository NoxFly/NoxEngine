// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/Camera.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace Nox {

    PerspectiveCamera::PerspectiveCamera(float fovDegrees, float aspect, float nearPlane, float farPlane)
        : fovRadians_(glm::radians(fovDegrees))
        , aspect_(aspect)
        , near_(nearPlane)
        , far_(farPlane) {}

    void PerspectiveCamera::setPosition(float x, float y, float z) {
        position_ = { x, y, z };
        viewDirty_ = true;
    }

    void PerspectiveCamera::setPosition(const Math::Vec3& pos) {
        position_ = pos;
        viewDirty_ = true;
    }

    void PerspectiveCamera::lookAt(float x, float y, float z) {
        target_ = { x, y, z };
        viewDirty_ = true;
    }

    void PerspectiveCamera::lookAt(const Math::Vec3& target) {
        target_ = target;
        viewDirty_ = true;
    }

    void PerspectiveCamera::setAspect(float aspect) {
        aspect_ = aspect;
        projDirty_ = true;
    }

    void PerspectiveCamera::setFov(float fovDegrees) {
        fovRadians_ = glm::radians(fovDegrees);
        projDirty_ = true;
    }

    const Math::Mat4& PerspectiveCamera::viewMatrix() const {
        if (viewDirty_) updateView();
        return viewMatrix_;
    }

    const Math::Mat4& PerspectiveCamera::projectionMatrix() const {
        if (projDirty_) updateProjection();
        return projMatrix_;
    }

    void PerspectiveCamera::updateView() const {
        viewMatrix_ = glm::lookAt(position_, target_, up_);
        viewDirty_ = false;
    }

    void PerspectiveCamera::updateProjection() const {
        projMatrix_ = glm::perspective(fovRadians_, aspect_, near_, far_);
        projDirty_ = false;
    }

} // namespace Nox

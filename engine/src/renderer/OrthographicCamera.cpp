// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/OrthographicCamera.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace Nox {

    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float nearPlane, float farPlane)
        : left_(left)
        , right_(right)
        , bottom_(bottom)
        , top_(top)
        , near_(nearPlane)
        , far_(farPlane) {}

    void OrthographicCamera::setPosition(float x, float y, float z) {
        position_ = { x, y, z };
        viewDirty_ = true;
    }

    void OrthographicCamera::setPosition(const Math::Vec3& pos) {
        position_ = pos;
        viewDirty_ = true;
    }

    void OrthographicCamera::lookAt(float x, float y, float z) {
        target_ = { x, y, z };
        viewDirty_ = true;
    }

    void OrthographicCamera::lookAt(const Math::Vec3& target) {
        target_ = target;
        viewDirty_ = true;
    }

    void OrthographicCamera::setBounds(float left, float right, float bottom, float top) {
        left_   = left;
        right_  = right;
        bottom_ = bottom;
        top_    = top;
        projDirty_ = true;
    }

    const Math::Mat4& OrthographicCamera::viewMatrix() const {
        if (viewDirty_) { updateView(); }
        return viewMatrix_;
    }

    const Math::Mat4& OrthographicCamera::projectionMatrix() const {
        if (projDirty_) { updateProjection(); }
        return projMatrix_;
    }

    void OrthographicCamera::updateView() const {
        viewMatrix_ = glm::lookAt(position_, target_, up_);
        viewDirty_ = false;
    }

    void OrthographicCamera::updateProjection() const {
        projMatrix_ = glm::ortho(left_, right_, bottom_, top_, near_, far_);
        projDirty_ = false;
    }

} // namespace Nox

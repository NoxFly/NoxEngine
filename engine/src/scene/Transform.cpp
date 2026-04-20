// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/scene/Transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Nox {

    void Transform::setPosition(const Math::Vec3& pos) {
        position_ = pos;
        dirty_ = true;
    }

    void Transform::setRotation(const Math::Quat& rot) {
        rotation_ = rot;
        dirty_ = true;
    }

    void Transform::setScale(const Math::Vec3& scl) {
        scale_ = scl;
        dirty_ = true;
    }

    Math::Mat4 Transform::localMatrix() const {
        if (dirty_) {
            Math::Mat4 t = glm::translate(Math::Mat4(1.0f), position_);
            Math::Mat4 r = glm::toMat4(rotation_);
            Math::Mat4 s = glm::scale(Math::Mat4(1.0f), scale_);
            cachedLocal_ = t * r * s;
            dirty_ = false;
        }
        return cachedLocal_;
    }

    Math::Mat4 Transform::computeWorld(const Math::Mat4& parentWorld) const {
        return parentWorld * localMatrix();
    }

} // namespace Nox

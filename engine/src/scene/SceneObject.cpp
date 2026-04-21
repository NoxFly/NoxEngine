// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/scene/SceneObject.hpp>

#include <glm/gtc/quaternion.hpp>

namespace Nox {

    SceneObject::SceneObject(std::string name, SceneObjectType type)
        : name_(std::move(name))
        , objectType_(type) {}

    void SceneObject::setPosition(float x, float y, float z) {
        transform_.setPosition({ x, y, z });
    }

    void SceneObject::setPosition(const Math::Vec3& pos) {
        transform_.setPosition(pos);
    }

    void SceneObject::setRotation(float xDeg, float yDeg, float zDeg) {
        Math::Quat q = Math::Quat(glm::radians(Math::Vec3(xDeg, yDeg, zDeg)));
        transform_.setRotation(q);
    }

    void SceneObject::setScale(float uniform) {
        transform_.setScale({ uniform, uniform, uniform });
    }

    void SceneObject::setScale(float x, float y, float z) {
        transform_.setScale({ x, y, z });
    }

    void SceneObject::rotate(float xDeg, float yDeg, float zDeg) {
        Math::Quat delta = Math::Quat(glm::radians(Math::Vec3(xDeg, yDeg, zDeg)));
        transform_.setRotation(delta * transform_.rotation());
    }

    void SceneObject::translate(float x, float y, float z) {
        transform_.setPosition(transform_.position() + Math::Vec3(x, y, z));
    }

    void SceneObject::updateWorldMatrix(const Math::Mat4& parentWorld) {
        worldMatrix_ = transform_.computeWorld(parentWorld);
    }

} // namespace Nox

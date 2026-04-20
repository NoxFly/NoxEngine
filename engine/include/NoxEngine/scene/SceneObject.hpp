// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/core/Color.hpp>
#include <NoxEngine/math/Types.hpp>
#include <NoxEngine/scene/Transform.hpp>

#include <memory>
#include <string>

namespace Nox {

    class SceneObject : public std::enable_shared_from_this<SceneObject> {
    public:
        explicit SceneObject(std::string name = "Object");
        virtual ~SceneObject() = default;

        SceneObject(const SceneObject&) = delete;
        SceneObject& operator=(const SceneObject&) = delete;
        SceneObject(SceneObject&&) = default;
        SceneObject& operator=(SceneObject&&) = default;

        [[nodiscard]] const std::string& name() const { return name_; }
        void setName(std::string name) { name_ = std::move(name); }

        // ── Transform shortcuts (angles in degrees) ────────────────
        void setPosition(float x, float y, float z);
        void setPosition(const Math::Vec3& pos);
        void setRotation(float xDeg, float yDeg, float zDeg);
        void setScale(float uniform);
        void setScale(float x, float y, float z);

        void rotate(float xDeg, float yDeg, float zDeg);
        void translate(float x, float y, float z);

        Transform&       transform()       { return transform_; }
        const Transform& transform() const { return transform_; }
        const Math::Mat4& worldMatrix() const { return worldMatrix_; }

        virtual void updateWorldMatrix(const Math::Mat4& parentWorld = Math::Mat4(1.0f));

    private:
        std::string name_;
        Transform   transform_;
        Math::Mat4  worldMatrix_{ 1.0f };
    };

} // namespace Nox

// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/core/Color.hpp>
#include <NoxEngine/math/Types.hpp>
#include <NoxEngine/scene/Transform.hpp>

#include <cstdint>
#include <memory>
#include <string>

namespace Nox {

    /// Runtime type tag for SceneObject subclasses.
    /// Avoids dynamic_cast / RTTI in hot paths (Scene3D::add, renderInternal).
    enum class SceneObjectType : uint8_t {
        Unknown,
        Mesh,
        Light,              ///< Generic light (base)
        DirectionalLight,
        PointLight,
        AmbientLight,
    };

    class SceneObject : public std::enable_shared_from_this<SceneObject> {
    public:
        explicit SceneObject(std::string name = "Object",
                             SceneObjectType type = SceneObjectType::Unknown);
        virtual ~SceneObject() = default;

        SceneObject(const SceneObject&) = delete;
        SceneObject& operator=(const SceneObject&) = delete;
        SceneObject(SceneObject&&) = default;
        SceneObject& operator=(SceneObject&&) = default;

        [[nodiscard]] SceneObjectType objectType() const { return objectType_; }

        /// Returns true if this object is a Light or any Light subtype.
        [[nodiscard]] bool isLight() const {
            return objectType_ == SceneObjectType::Light
                || objectType_ == SceneObjectType::DirectionalLight
                || objectType_ == SceneObjectType::PointLight
                || objectType_ == SceneObjectType::AmbientLight;
        }

        /// Returns true if this object is a Mesh.
        [[nodiscard]] bool isMesh() const {
            return objectType_ == SceneObjectType::Mesh;
        }

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
        std::string     name_;
        SceneObjectType objectType_;
        Transform       transform_;
        Math::Mat4      worldMatrix_{ 1.0f };
    };

} // namespace Nox

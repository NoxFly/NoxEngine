// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

namespace Nox {

    class Transform {
    public:
        Transform() = default;

        void setPosition(const Math::Vec3& pos);
        void setRotation(const Math::Quat& rot);
        void setScale(const Math::Vec3& scl);

        [[nodiscard]] const Math::Vec3& position() const { return position_; }
        [[nodiscard]] const Math::Quat& rotation() const { return rotation_; }
        [[nodiscard]] const Math::Vec3& scale()    const { return scale_; }

        [[nodiscard]] Math::Mat4 localMatrix() const;
        [[nodiscard]] Math::Mat4 computeWorld(const Math::Mat4& parentWorld) const;

        [[nodiscard]] bool isDirty() const { return dirty_; }
        void clearDirty() { dirty_ = false; }

    private:
        Math::Vec3 position_{ 0.0f, 0.0f, 0.0f };
        Math::Quat rotation_{ 1.0f, 0.0f, 0.0f, 0.0f };
        Math::Vec3 scale_{ 1.0f, 1.0f, 1.0f };
        mutable bool dirty_ = true;
        mutable Math::Mat4 cachedLocal_{ 1.0f };
    };

} // namespace Nox

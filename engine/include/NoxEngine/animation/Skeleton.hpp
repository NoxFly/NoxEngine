// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace Nox {

    struct Joint {
        std::string name;
        int32_t     parentIndex = -1;
        Math::Mat4  inverseBindMatrix{ 1.0f };
        Math::Mat4  localTransform{ 1.0f };
    };

    class Skeleton {
    public:
        Skeleton() = default;

        void addJoint(Joint joint);

        [[nodiscard]] const std::vector<Joint>& joints() const { return joints_; }
        [[nodiscard]] std::vector<Joint>& joints() { return joints_; }
        [[nodiscard]] uint32_t jointCount() const { return static_cast<uint32_t>(joints_.size()); }

        [[nodiscard]] int32_t findJointIndex(std::string_view name) const;

        // Compute final bone matrices (model-space * inverse bind)
        [[nodiscard]] std::vector<Math::Mat4> computeBoneMatrices() const;

        static constexpr uint32_t MaxJoints = 128;

    private:
        std::vector<Joint> joints_;
    };

} // namespace Nox

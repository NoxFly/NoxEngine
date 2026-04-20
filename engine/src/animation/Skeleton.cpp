// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/animation/Skeleton.hpp>

#include <algorithm>

namespace Nox {

    void Skeleton::addJoint(Joint joint) {
        joints_.push_back(std::move(joint));
    }

    int32_t Skeleton::findJointIndex(std::string_view name) const {
        for (size_t i = 0; i < joints_.size(); ++i) {
            if (joints_[i].name == name) {
                return static_cast<int32_t>(i);
            }
        }
        return -1;
    }

    std::vector<Math::Mat4> Skeleton::computeBoneMatrices() const {
        std::vector<Math::Mat4> worldTransforms(joints_.size(), Math::Mat4(1.0f));
        std::vector<Math::Mat4> boneMatrices(joints_.size());

        for (size_t i = 0; i < joints_.size(); ++i) {
            if (joints_[i].parentIndex >= 0) {
                worldTransforms[i] = worldTransforms[static_cast<size_t>(joints_[i].parentIndex)]
                                   * joints_[i].localTransform;
            }
            else {
                worldTransforms[i] = joints_[i].localTransform;
            }
            boneMatrices[i] = worldTransforms[i] * joints_[i].inverseBindMatrix;
        }

        return boneMatrices;
    }

} // namespace Nox

// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/animation/AnimationClip.hpp>
#include <NoxEngine/animation/Skeleton.hpp>

#include <memory>
#include <vector>

namespace Nox {

    enum class AnimationState { Stopped, Playing, Paused };

    class AnimationPlayer {
    public:
        AnimationPlayer() = default;

        void setSkeleton(std::shared_ptr<Skeleton> skeleton);
        void setClip(std::shared_ptr<AnimationClip> clip);

        void play();
        void pause();
        void stop();

        void setLooping(bool loop) { looping_ = loop; }
        void setSpeed(float speed) { speed_ = speed; }

        [[nodiscard]] bool isLooping() const { return looping_; }
        [[nodiscard]] float speed() const { return speed_; }
        [[nodiscard]] float currentTime() const { return currentTime_; }
        [[nodiscard]] AnimationState state() const { return state_; }

        // Cross-fade to a new clip over fadeTime seconds
        void crossFadeTo(std::shared_ptr<AnimationClip> clip, float fadeTime);

        // Advance time and compute bone matrices
        void update(float dt);

        // Get the final bone matrices (skeleton space * inverse bind)
        [[nodiscard]] const std::vector<Math::Mat4>& boneMatrices() const { return boneMatrices_; }

        // Get morph weights
        [[nodiscard]] const std::vector<float>& morphWeights() const { return morphWeights_; }

    private:
        void applyClipToSkeleton(const AnimationClip& clip, float time, float weight);

        std::shared_ptr<Skeleton>      skeleton_;
        std::shared_ptr<AnimationClip> currentClip_;
        std::shared_ptr<AnimationClip> fadeClip_;

        AnimationState state_ = AnimationState::Stopped;
        float currentTime_ = 0.0f;
        float speed_       = 1.0f;
        bool  looping_     = true;

        // Cross-fade state
        float fadeTime_    = 0.0f;
        float fadeElapsed_ = 0.0f;
        float fadeClipTime_ = 0.0f;

        std::vector<Math::Mat4> boneMatrices_;
        std::vector<float>      morphWeights_;

        // Cached per-joint transforms for blending
        std::vector<Math::Vec3> positions_;
        std::vector<Math::Quat> rotations_;
        std::vector<Math::Vec3> scales_;
    };

} // namespace Nox

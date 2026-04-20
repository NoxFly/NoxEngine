// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/animation/AnimationPlayer.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <algorithm>

namespace Nox {

    void AnimationPlayer::setSkeleton(std::shared_ptr<Skeleton> skeleton) {
        skeleton_ = std::move(skeleton);
        if (skeleton_) {
            auto count = skeleton_->jointCount();
            positions_.resize(count, Math::Vec3(0.0f));
            rotations_.resize(count, Math::Quat(1.0f, 0.0f, 0.0f, 0.0f));
            scales_.resize(count, Math::Vec3(1.0f));
            boneMatrices_.resize(count, Math::Mat4(1.0f));
        }
    }

    void AnimationPlayer::setClip(std::shared_ptr<AnimationClip> clip) {
        currentClip_ = std::move(clip);
        currentTime_ = 0.0f;
        fadeClip_ = nullptr;
        fadeTime_ = 0.0f;
    }

    void AnimationPlayer::play() {
        state_ = AnimationState::Playing;
    }

    void AnimationPlayer::pause() {
        state_ = AnimationState::Paused;
    }

    void AnimationPlayer::stop() {
        state_ = AnimationState::Stopped;
        currentTime_ = 0.0f;
    }

    void AnimationPlayer::crossFadeTo(std::shared_ptr<AnimationClip> clip, float fadeTime) {
        if (!currentClip_) {
            setClip(std::move(clip));
            play();
            return;
        }
        fadeClip_ = std::move(clip);
        fadeTime_ = fadeTime;
        fadeElapsed_ = 0.0f;
        fadeClipTime_ = 0.0f;
    }

    void AnimationPlayer::update(float dt) {
        if (state_ != AnimationState::Playing || !skeleton_ || !currentClip_) {
            return;
        }

        currentTime_ += dt * speed_;

        if (!looping_ && currentTime_ >= currentClip_->duration()) {
            currentTime_ = currentClip_->duration();
            state_ = AnimationState::Stopped;
        }

        auto count = skeleton_->jointCount();

        // Reset to bind pose
        for (size_t i = 0; i < count; ++i) {
            positions_[i] = Math::Vec3(0.0f);
            rotations_[i] = Math::Quat(1.0f, 0.0f, 0.0f, 0.0f);
            scales_[i]    = Math::Vec3(1.0f);
        }

        // Sample current clip
        currentClip_->sample(currentTime_, positions_, rotations_, scales_);

        // Handle cross-fade blending
        if (fadeClip_ && fadeTime_ > 0.0f) {
            fadeElapsed_ += dt * speed_;
            fadeClipTime_ += dt * speed_;
            float blendFactor = std::clamp(fadeElapsed_ / fadeTime_, 0.0f, 1.0f);

            std::vector<Math::Vec3> fadePositions(count, Math::Vec3(0.0f));
            std::vector<Math::Quat> fadeRotations(count, Math::Quat(1.0f, 0.0f, 0.0f, 0.0f));
            std::vector<Math::Vec3> fadeScales(count, Math::Vec3(1.0f));

            fadeClip_->sample(fadeClipTime_, fadePositions, fadeRotations, fadeScales);

            // Blend between current and fade clip
            for (size_t i = 0; i < count; ++i) {
                positions_[i] = glm::mix(positions_[i], fadePositions[i], blendFactor);
                rotations_[i] = glm::slerp(rotations_[i], fadeRotations[i], blendFactor);
                scales_[i]    = glm::mix(scales_[i], fadeScales[i], blendFactor);
            }

            // Fade complete — switch to new clip
            if (blendFactor >= 1.0f) {
                currentClip_ = fadeClip_;
                currentTime_ = fadeClipTime_;
                fadeClip_ = nullptr;
                fadeTime_ = 0.0f;
            }
        }

        // Build joint-local transforms and compute bone matrices
        auto& joints = skeleton_->joints();
        for (size_t i = 0; i < count; ++i) {
            Math::Mat4 T = glm::translate(Math::Mat4(1.0f), positions_[i]);
            Math::Mat4 R = glm::toMat4(rotations_[i]);
            Math::Mat4 S = glm::scale(Math::Mat4(1.0f), scales_[i]);
            joints[i].localTransform = T * R * S;
        }

        boneMatrices_ = skeleton_->computeBoneMatrices();

        // Sample morph weights
        if (!currentClip_->morphChannels().empty()) {
            currentClip_->sampleMorphWeights(currentTime_, morphWeights_);
        }
    }

} // namespace Nox

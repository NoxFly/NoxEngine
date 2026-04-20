// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/animation/AnimationClip.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <algorithm>

namespace Nox {

    namespace {

        template<typename T>
        [[nodiscard]] size_t findKeyIndex(const std::vector<Keyframe<T>>& keys, float time) {
            for (size_t i = 0; i + 1 < keys.size(); ++i) {
                if (time < keys[i + 1].time) {
                    return i;
                }
            }
            return keys.empty() ? 0 : keys.size() - 1;
        }

        [[nodiscard]] Math::Vec3 interpolateVec3(
            const std::vector<Keyframe<Math::Vec3>>& keys,
            float time, Interpolation interp)
        {
            if (keys.empty()) { return Math::Vec3(0.0f); }
            if (keys.size() == 1) { return keys[0].value; }

            size_t i = findKeyIndex(keys, time);
            if (i + 1 >= keys.size()) { return keys.back().value; }

            if (interp == Interpolation::Step) {
                return keys[i].value;
            }

            float t0 = keys[i].time;
            float t1 = keys[i + 1].time;
            float factor = (t1 > t0) ? (time - t0) / (t1 - t0) : 0.0f;
            factor = std::clamp(factor, 0.0f, 1.0f);

            return glm::mix(keys[i].value, keys[i + 1].value, factor);
        }

        [[nodiscard]] Math::Quat interpolateQuat(
            const std::vector<Keyframe<Math::Quat>>& keys,
            float time, Interpolation interp)
        {
            if (keys.empty()) { return Math::Quat(1.0f, 0.0f, 0.0f, 0.0f); }
            if (keys.size() == 1) { return keys[0].value; }

            size_t i = findKeyIndex(keys, time);
            if (i + 1 >= keys.size()) { return keys.back().value; }

            if (interp == Interpolation::Step) {
                return keys[i].value;
            }

            float t0 = keys[i].time;
            float t1 = keys[i + 1].time;
            float factor = (t1 > t0) ? (time - t0) / (t1 - t0) : 0.0f;
            factor = std::clamp(factor, 0.0f, 1.0f);

            return glm::slerp(keys[i].value, keys[i + 1].value, factor);
        }

    } // anonymous namespace

    AnimationClip::AnimationClip(std::string name)
        : name_(std::move(name)) {}

    void AnimationClip::addChannel(AnimationChannel channel) {
        channels_.push_back(std::move(channel));
    }

    void AnimationClip::addMorphChannel(MorphChannel channel) {
        morphChannels_.push_back(std::move(channel));
    }

    void AnimationClip::sample(float time,
                               std::vector<Math::Vec3>& outPositions,
                               std::vector<Math::Quat>& outRotations,
                               std::vector<Math::Vec3>& outScales) const
    {
        float t = std::fmod(time, duration_);
        if (t < 0.0f) { t += duration_; }

        for (const auto& ch : channels_) {
            if (ch.jointIndex < 0) { continue; }
            auto idx = static_cast<size_t>(ch.jointIndex);

            if (idx < outPositions.size() && !ch.positionKeys.empty()) {
                outPositions[idx] = interpolateVec3(ch.positionKeys, t, ch.interpolation);
            }
            if (idx < outRotations.size() && !ch.rotationKeys.empty()) {
                outRotations[idx] = interpolateQuat(ch.rotationKeys, t, ch.interpolation);
            }
            if (idx < outScales.size() && !ch.scaleKeys.empty()) {
                outScales[idx] = interpolateVec3(ch.scaleKeys, t, ch.interpolation);
            }
        }
    }

    void AnimationClip::sampleMorphWeights(float time, std::vector<float>& outWeights) const {
        float t = std::fmod(time, duration_);
        if (t < 0.0f) { t += duration_; }

        for (const auto& ch : morphChannels_) {
            if (ch.weightKeys.empty()) { continue; }

            size_t i = findKeyIndex(ch.weightKeys, t);
            const auto& weights = ch.weightKeys[i].value;

            for (size_t w = 0; w < weights.size() && w < outWeights.size(); ++w) {
                if (ch.interpolation == Interpolation::Step || i + 1 >= ch.weightKeys.size()) {
                    outWeights[w] = weights[w];
                }
                else {
                    float t0 = ch.weightKeys[i].time;
                    float t1 = ch.weightKeys[i + 1].time;
                    float factor = (t1 > t0) ? (t - t0) / (t1 - t0) : 0.0f;
                    factor = std::clamp(factor, 0.0f, 1.0f);
                    outWeights[w] = glm::mix(weights[w], ch.weightKeys[i + 1].value[w], factor);
                }
            }
        }
    }

} // namespace Nox

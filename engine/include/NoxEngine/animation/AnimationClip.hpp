// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace Nox {

    // A single keyframe for one property channel
    template<typename T>
    struct Keyframe {
        float time = 0.0f;
        T     value{};
    };

    // Interpolation mode
    enum class Interpolation { Linear, Step, CubicSpline };

    // A channel targets one joint's position, rotation, or scale
    struct AnimationChannel {
        int32_t       jointIndex = -1;
        Interpolation interpolation = Interpolation::Linear;

        std::vector<Keyframe<Math::Vec3>> positionKeys;
        std::vector<Keyframe<Math::Quat>> rotationKeys;
        std::vector<Keyframe<Math::Vec3>> scaleKeys;
    };

    // Morph target keyframes
    struct MorphChannel {
        uint32_t meshIndex = 0;
        Interpolation interpolation = Interpolation::Linear;
        std::vector<Keyframe<std::vector<float>>> weightKeys;
    };

    class AnimationClip {
    public:
        explicit AnimationClip(std::string name = "");

        [[nodiscard]] const std::string& name() const { return name_; }
        void setName(std::string name) { name_ = std::move(name); }

        [[nodiscard]] float duration() const { return duration_; }
        void setDuration(float d) { duration_ = d; }

        void addChannel(AnimationChannel channel);
        void addMorphChannel(MorphChannel channel);

        [[nodiscard]] const std::vector<AnimationChannel>& channels() const { return channels_; }
        [[nodiscard]] const std::vector<MorphChannel>& morphChannels() const { return morphChannels_; }

        // Sample all channels at a given time, writing joint-local transforms
        void sample(float time,
                    std::vector<Math::Vec3>& outPositions,
                    std::vector<Math::Quat>& outRotations,
                    std::vector<Math::Vec3>& outScales) const;

        // Sample morph weights at a given time
        void sampleMorphWeights(float time, std::vector<float>& outWeights) const;

    private:
        std::string name_;
        float       duration_ = 0.0f;
        std::vector<AnimationChannel> channels_;
        std::vector<MorphChannel>     morphChannels_;
    };

} // namespace Nox

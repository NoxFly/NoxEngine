// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/renderer/PostProcessEffect.hpp>

#include <cstdint>
#include <string_view>

namespace Nox {

    class BloomEffect : public PostProcessEffect {
    public:
        BloomEffect() = default;
        ~BloomEffect() override;

        void init(int width, int height) override;
        void resize(int width, int height) override;
        void apply(uint32_t inputTexture, uint32_t outputFBO, int width, int height) override;

        [[nodiscard]] std::string_view name() const override { return "Bloom"; }

        void setThreshold(float threshold) { threshold_ = threshold; }
        void setIntensity(float intensity) { intensity_ = intensity; }
        void setIterations(int iterations) { iterations_ = iterations; }

        [[nodiscard]] float threshold() const { return threshold_; }
        [[nodiscard]] float intensity() const { return intensity_; }
        [[nodiscard]] int iterations() const { return iterations_; }

    private:
        void createBlurResources(int halfWidth, int halfHeight);
        void destroyResources();

        float threshold_  = 1.0f;
        float intensity_  = 1.0f;
        int   iterations_ = 3;

        uint32_t brightProgram_   = 0;
        uint32_t blurHProgram_    = 0;
        uint32_t blurVProgram_    = 0;
        uint32_t combineProgram_  = 0;

        uint32_t brightFBO_     = 0;
        uint32_t brightTexture_ = 0;

        uint32_t blurFBOs_[2]{ 0, 0 };
        uint32_t blurTextures_[2]{ 0, 0 };

        int halfWidth_  = 0;
        int halfHeight_ = 0;

        // Uniform locations
        int32_t brightLocTexture_   = -1;
        int32_t brightLocThreshold_ = -1;
        int32_t blurHLocTexture_    = -1;
        int32_t blurHLocTexelSize_  = -1;
        int32_t blurVLocTexture_    = -1;
        int32_t blurVLocTexelSize_  = -1;
        int32_t combLocScene_       = -1;
        int32_t combLocBloom_       = -1;
        int32_t combLocIntensity_   = -1;

        static constexpr std::string_view BrightExtractSource = R"glsl(
        #version 460 core
        in vec2 vUV;
        out vec4 fragColor;
        uniform sampler2D uTexture;
        uniform float uThreshold;
        void main() {
            vec3 color = texture(uTexture, vUV).rgb;
            float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
            fragColor = (brightness > uThreshold) ? vec4(color, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);
        }
        )glsl";

        static constexpr std::string_view BlurHorizontalSource = R"glsl(
        #version 460 core
        in vec2 vUV;
        out vec4 fragColor;
        uniform sampler2D uTexture;
        uniform float uTexelSize;
        const float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
        void main() {
            vec3 result = texture(uTexture, vUV).rgb * weights[0];
            for (int i = 1; i < 5; ++i) {
                float offset = float(i) * uTexelSize;
                result += texture(uTexture, vUV + vec2(offset, 0.0)).rgb * weights[i];
                result += texture(uTexture, vUV - vec2(offset, 0.0)).rgb * weights[i];
            }
            fragColor = vec4(result, 1.0);
        }
        )glsl";

        static constexpr std::string_view BlurVerticalSource = R"glsl(
        #version 460 core
        in vec2 vUV;
        out vec4 fragColor;
        uniform sampler2D uTexture;
        uniform float uTexelSize;
        const float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
        void main() {
            vec3 result = texture(uTexture, vUV).rgb * weights[0];
            for (int i = 1; i < 5; ++i) {
                float offset = float(i) * uTexelSize;
                result += texture(uTexture, vUV + vec2(0.0, offset)).rgb * weights[i];
                result += texture(uTexture, vUV - vec2(0.0, offset)).rgb * weights[i];
            }
            fragColor = vec4(result, 1.0);
        }
        )glsl";

        static constexpr std::string_view CombineSource = R"glsl(
        #version 460 core
        in vec2 vUV;
        out vec4 fragColor;
        uniform sampler2D uScene;
        uniform sampler2D uBloom;
        uniform float uIntensity;
        void main() {
            vec3 scene = texture(uScene, vUV).rgb;
            vec3 bloom = texture(uBloom, vUV).rgb;
            fragColor = vec4(scene + bloom * uIntensity, 1.0);
        }
        )glsl";
    };

} // namespace Nox

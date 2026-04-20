// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/renderer/PostProcessEffect.hpp>

#include <cstdint>
#include <string_view>

namespace Nox {

    class SSAOEffect : public PostProcessEffect {
    public:
        SSAOEffect() = default;
        ~SSAOEffect() override;

        void init(int width, int height) override;
        void resize(int width, int height) override;
        void apply(uint32_t inputTexture, uint32_t outputFBO, int width, int height) override;

        [[nodiscard]] std::string_view name() const override { return "SSAO"; }

        void setDepthTexture(uint32_t depthTex) { depthTexture_ = depthTex; }
        void setProjectionMatrix(const float* projMatrix) { projMatrix_ = projMatrix; }

        void setRadius(float radius) { radius_ = radius; }
        void setBias(float bias) { bias_ = bias; }
        void setKernelSize(int size) { kernelSize_ = size; }
        void setOcclusionPower(float power) { occlusionPower_ = power; }

        [[nodiscard]] float radius() const { return radius_; }
        [[nodiscard]] float bias() const { return bias_; }
        [[nodiscard]] int kernelSize() const { return kernelSize_; }

    private:
        void createResources(int width, int height);
        void destroyResources();
        void generateKernel();
        void generateNoiseTexture();

        uint32_t depthTexture_ = 0;
        const float* projMatrix_ = nullptr;

        float radius_ = 0.5f;
        float bias_   = 0.025f;
        int   kernelSize_ = 16;
        float occlusionPower_ = 1.5f;

        uint32_t ssaoProgram_    = 0;
        uint32_t blurProgram_    = 0;
        uint32_t combineProgram_ = 0;

        uint32_t ssaoFBO_     = 0;
        uint32_t ssaoTexture_ = 0;
        uint32_t blurFBO_     = 0;
        uint32_t blurTexture_ = 0;
        uint32_t noiseTexture_ = 0;

        int width_  = 0;
        int height_ = 0;

        // Uniform locations — SSAO pass
        int32_t ssaoLocDepth_      = -1;
        int32_t ssaoLocNoise_      = -1;
        int32_t ssaoLocProjection_ = -1;
        int32_t ssaoLocSamples_    = -1;
        int32_t ssaoLocRadius_     = -1;
        int32_t ssaoLocBias_       = -1;
        int32_t ssaoLocScreenSize_ = -1;
        int32_t ssaoLocKernelSize_ = -1;
        int32_t ssaoLocPower_      = -1;

        // Uniform locations — blur pass
        int32_t blurLocTexture_ = -1;

        // Uniform locations — combine pass
        int32_t combLocScene_ = -1;
        int32_t combLocAO_    = -1;

        static constexpr int MaxKernelSize = 64;
        float kernelData_[MaxKernelSize * 3]{};

        static constexpr std::string_view SSAOFragSource = R"glsl(
        #version 460 core
        in vec2 vUV;
        out float fragColor;

        uniform sampler2D uDepth;
        uniform sampler2D uNoise;
        uniform mat4 uProjection;
        uniform vec3 uSamples[64];
        uniform float uRadius;
        uniform float uBias;
        uniform vec2 uScreenSize;
        uniform int uKernelSize;
        uniform float uPower;

        vec3 reconstructViewPos(vec2 uv, float depth) {
            float z = depth * 2.0 - 1.0;
            vec4 clipPos = vec4(uv * 2.0 - 1.0, z, 1.0);
            vec4 viewPos = inverse(uProjection) * clipPos;
            return viewPos.xyz / viewPos.w;
        }

        void main() {
            float depth = texture(uDepth, vUV).r;
            if (depth >= 1.0) {
                fragColor = 1.0;
                return;
            }

            vec3 fragPos = reconstructViewPos(vUV, depth);

            // Reconstruct normal from depth via cross product of partial derivatives
            vec2 texelSize = 1.0 / uScreenSize;
            float depthR = texture(uDepth, vUV + vec2(texelSize.x, 0.0)).r;
            float depthU = texture(uDepth, vUV + vec2(0.0, texelSize.y)).r;
            vec3 posR = reconstructViewPos(vUV + vec2(texelSize.x, 0.0), depthR);
            vec3 posU = reconstructViewPos(vUV + vec2(0.0, texelSize.y), depthU);
            vec3 normal = normalize(cross(posR - fragPos, posU - fragPos));

            // Noise for random rotation of kernel
            vec2 noiseScale = uScreenSize / 4.0;
            vec3 randomVec = texture(uNoise, vUV * noiseScale).rgb;

            // Gram-Schmidt to build TBN
            vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
            vec3 bitangent = cross(normal, tangent);
            mat3 TBN = mat3(tangent, bitangent, normal);

            float occlusion = 0.0;
            for (int i = 0; i < uKernelSize; ++i) {
                vec3 samplePos = fragPos + TBN * uSamples[i] * uRadius;

                // Project sample to screen space
                vec4 offset = uProjection * vec4(samplePos, 1.0);
                offset.xyz /= offset.w;
                offset.xyz = offset.xyz * 0.5 + 0.5;

                float sampleDepth = texture(uDepth, offset.xy).r;
                vec3 sampleViewPos = reconstructViewPos(offset.xy, sampleDepth);

                float rangeCheck = smoothstep(0.0, 1.0, uRadius / abs(fragPos.z - sampleViewPos.z));
                occlusion += (sampleViewPos.z >= samplePos.z + uBias ? 1.0 : 0.0) * rangeCheck;
            }

            occlusion = 1.0 - (occlusion / float(uKernelSize));
            fragColor = pow(occlusion, uPower);
        }
        )glsl";

        static constexpr std::string_view BlurFragSource = R"glsl(
        #version 460 core
        in vec2 vUV;
        out float fragColor;
        uniform sampler2D uTexture;
        void main() {
            vec2 texelSize = 1.0 / vec2(textureSize(uTexture, 0));
            float result = 0.0;
            for (int x = -2; x <= 2; ++x) {
                for (int y = -2; y <= 2; ++y) {
                    result += texture(uTexture, vUV + vec2(float(x), float(y)) * texelSize).r;
                }
            }
            fragColor = result / 25.0;
        }
        )glsl";

        static constexpr std::string_view CombineFragSource = R"glsl(
        #version 460 core
        in vec2 vUV;
        out vec4 fragColor;
        uniform sampler2D uScene;
        uniform sampler2D uAO;
        void main() {
            vec3 scene = texture(uScene, vUV).rgb;
            float ao = texture(uAO, vUV).r;
            fragColor = vec4(scene * ao, 1.0);
        }
        )glsl";
    };

} // namespace Nox

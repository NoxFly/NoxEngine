// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/renderer/PostProcessEffect.hpp>

#include <cstdint>
#include <string_view>

namespace Nox {

    class FXAAEffect : public PostProcessEffect {
    public:
        FXAAEffect() = default;
        ~FXAAEffect() override;

        void init(int width, int height) override;
        void resize(int width, int height) override;
        void apply(uint32_t inputTexture, uint32_t outputFBO, int width, int height) override;

        [[nodiscard]] std::string_view name() const override { return "FXAA"; }

    private:
        uint32_t program_ = 0;
        int32_t locTexture_ = -1;
        int32_t locInverseScreenSize_ = -1;

        static constexpr std::string_view FragmentSource = R"glsl(
        #version 460 core

        in vec2 vUV;
        out vec4 fragColor;

        uniform sampler2D uTexture;
        uniform vec2 uInverseScreenSize;

        const float ReduceMin = 1.0 / 128.0;
        const float ReduceMul = 1.0 / 8.0;
        const float SpanMax   = 8.0;

        void main() {
            vec3 rgbNW = texture(uTexture, vUV + vec2(-1.0, -1.0) * uInverseScreenSize).rgb;
            vec3 rgbNE = texture(uTexture, vUV + vec2( 1.0, -1.0) * uInverseScreenSize).rgb;
            vec3 rgbSW = texture(uTexture, vUV + vec2(-1.0,  1.0) * uInverseScreenSize).rgb;
            vec3 rgbSE = texture(uTexture, vUV + vec2( 1.0,  1.0) * uInverseScreenSize).rgb;
            vec3 rgbM  = texture(uTexture, vUV).rgb;

            vec3 lumaCoeff = vec3(0.299, 0.587, 0.114);
            float lumaNW = dot(rgbNW, lumaCoeff);
            float lumaNE = dot(rgbNE, lumaCoeff);
            float lumaSW = dot(rgbSW, lumaCoeff);
            float lumaSE = dot(rgbSE, lumaCoeff);
            float lumaM  = dot(rgbM,  lumaCoeff);

            float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
            float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

            vec2 dir;
            dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
            dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

            float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * ReduceMul, ReduceMin);
            float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
            dir = min(vec2(SpanMax), max(vec2(-SpanMax), dir * rcpDirMin)) * uInverseScreenSize;

            vec3 rgbA = 0.5 * (
                texture(uTexture, vUV + dir * (1.0 / 3.0 - 0.5)).rgb +
                texture(uTexture, vUV + dir * (2.0 / 3.0 - 0.5)).rgb
            );

            vec3 rgbB = rgbA * 0.5 + 0.25 * (
                texture(uTexture, vUV + dir * -0.5).rgb +
                texture(uTexture, vUV + dir *  0.5).rgb
            );

            float lumaB = dot(rgbB, lumaCoeff);

            if (lumaB < lumaMin || lumaB > lumaMax) {
                fragColor = vec4(rgbA, 1.0);
            }
            else {
                fragColor = vec4(rgbB, 1.0);
            }
        }
        )glsl";
    };

} // namespace Nox

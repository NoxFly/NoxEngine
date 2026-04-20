// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <cstdint>
#include <string_view>

namespace Nox {

    class PostProcessEffect {
    public:
        virtual ~PostProcessEffect() = default;

        PostProcessEffect(const PostProcessEffect&) = delete;
        PostProcessEffect& operator=(const PostProcessEffect&) = delete;
        PostProcessEffect(PostProcessEffect&&) = default;
        PostProcessEffect& operator=(PostProcessEffect&&) = default;

        virtual void init(int width, int height) = 0;
        virtual void resize(int width, int height) = 0;
        virtual void apply(uint32_t inputTexture, uint32_t outputFBO, int width, int height) = 0;

        void setEnabled(bool enabled) { enabled_ = enabled; }
        [[nodiscard]] bool isEnabled() const { return enabled_; }
        [[nodiscard]] virtual std::string_view name() const = 0;

    protected:
        PostProcessEffect() = default;

        bool enabled_ = true;

        static uint32_t compilePostProcessShader(std::string_view fragmentSource);
        static void drawFullscreenQuad();

        static constexpr std::string_view QuadVertexSource = R"glsl(
        #version 460 core
        layout(location = 0) in vec2 aPosition;
        layout(location = 1) in vec2 aUV;
        out vec2 vUV;
        void main() {
            vUV = aUV;
            gl_Position = vec4(aPosition, 0.0, 1.0);
        }
        )glsl";
    };

} // namespace Nox

// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/renderer/PostProcessEffect.hpp>

#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>

namespace Nox {

    class PostProcessStack {
    public:
        PostProcessStack() = default;
        ~PostProcessStack();

        PostProcessStack(const PostProcessStack&) = delete;
        PostProcessStack& operator=(const PostProcessStack&) = delete;
        PostProcessStack(PostProcessStack&&) = default;
        PostProcessStack& operator=(PostProcessStack&&) = default;

        void addEffect(std::unique_ptr<PostProcessEffect> effect);
        void removeEffect(std::string_view name);

        [[nodiscard]] PostProcessEffect* getEffect(std::string_view name);
        [[nodiscard]] const PostProcessEffect* getEffect(std::string_view name) const;

        void init(int width, int height);
        void resize(int width, int height);

        // Apply all enabled effects. Returns the texture containing the final result.
        // If no effects are active, returns inputTexture unchanged.
        [[nodiscard]] uint32_t apply(uint32_t inputTexture, int width, int height);

        [[nodiscard]] const std::vector<std::unique_ptr<PostProcessEffect>>& effects() const {
            return effects_;
        }

    private:
        void createPingPongResources(int width, int height);
        void destroyPingPongResources();

        std::vector<std::unique_ptr<PostProcessEffect>> effects_;

        uint32_t pingPongFBOs_[2]{ 0, 0 };
        uint32_t pingPongTextures_[2]{ 0, 0 };
        int width_  = 0;
        int height_ = 0;
    };

} // namespace Nox

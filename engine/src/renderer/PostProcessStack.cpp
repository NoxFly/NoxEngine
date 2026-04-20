// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/PostProcessStack.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

#include <algorithm>

namespace Nox {

    PostProcessStack::~PostProcessStack() {
        destroyPingPongResources();
    }

    void PostProcessStack::addEffect(std::unique_ptr<PostProcessEffect> effect) {
        effects_.push_back(std::move(effect));
    }

    void PostProcessStack::removeEffect(std::string_view name) {
        std::erase_if(effects_, [name](const std::unique_ptr<PostProcessEffect>& e) {
            return e->name() == name;
        });
    }

    PostProcessEffect* PostProcessStack::getEffect(std::string_view name) {
        auto it = std::ranges::find_if(effects_, [name](const std::unique_ptr<PostProcessEffect>& e) {
            return e->name() == name;
        });
        return it != effects_.end() ? it->get() : nullptr;
    }

    const PostProcessEffect* PostProcessStack::getEffect(std::string_view name) const {
        auto it = std::ranges::find_if(effects_, [name](const std::unique_ptr<PostProcessEffect>& e) {
            return e->name() == name;
        });
        return it != effects_.end() ? it->get() : nullptr;
    }

    void PostProcessStack::init(int width, int height) {
        createPingPongResources(width, height);

        for (auto& effect : effects_) {
            effect->init(width, height);
        }
    }

    void PostProcessStack::resize(int width, int height) {
        if (width == width_ && height == height_) {
            return;
        }

        destroyPingPongResources();
        createPingPongResources(width, height);

        for (auto& effect : effects_) {
            effect->resize(width, height);
        }
    }

    uint32_t PostProcessStack::apply(uint32_t inputTexture, int width, int height) {
        // Collect enabled effects
        std::vector<PostProcessEffect*> activeEffects;
        activeEffects.reserve(effects_.size());
        for (auto& effect : effects_) {
            if (effect->isEnabled()) {
                activeEffects.push_back(effect.get());
            }
        }

        if (activeEffects.empty()) {
            return inputTexture;
        }

        uint32_t currentInput = inputTexture;
        int pingPongIndex = 0;

        for (size_t i = 0; i < activeEffects.size(); ++i) {
            uint32_t targetFBO = pingPongFBOs_[pingPongIndex];

            activeEffects[i]->apply(currentInput, targetFBO, width, height);

            currentInput = pingPongTextures_[pingPongIndex];
            pingPongIndex = 1 - pingPongIndex;
        }

        return currentInput;
    }

    void PostProcessStack::createPingPongResources(int width, int height) {
        width_  = width;
        height_ = height;

        glCreateFramebuffers(2, pingPongFBOs_);
        glCreateTextures(GL_TEXTURE_2D, 2, pingPongTextures_);

        for (int i = 0; i < 2; ++i) {
            glTextureStorage2D(pingPongTextures_[i], 1, GL_RGBA16F, width, height);
            glTextureParameteri(pingPongTextures_[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(pingPongTextures_[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(pingPongTextures_[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(pingPongTextures_[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glNamedFramebufferTexture(pingPongFBOs_[i], GL_COLOR_ATTACHMENT0,
                                       pingPongTextures_[i], 0);
        }

        NOX_LOG_INFO("PostProcessStack ping-pong FBOs created ({}x{})", width, height);
    }

    void PostProcessStack::destroyPingPongResources() {
        if (pingPongFBOs_[0] != 0) {
            glDeleteFramebuffers(2, pingPongFBOs_);
            glDeleteTextures(2, pingPongTextures_);
            pingPongFBOs_[0] = 0;
            pingPongFBOs_[1] = 0;
            pingPongTextures_[0] = 0;
            pingPongTextures_[1] = 0;
        }
    }

} // namespace Nox

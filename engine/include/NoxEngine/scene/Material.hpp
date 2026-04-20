// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/core/Color.hpp>

#include <cstdint>
#include <memory>
#include <string>

namespace Nox {

    class Material {
    public:
        [[nodiscard]] static std::shared_ptr<Material> standard();
        [[nodiscard]] static std::shared_ptr<Material> unlit();

        void setColor(const Color& color) { color_ = color; }
        void setRoughness(float r) { roughness_ = r; }
        void setMetallic(float m) { metallic_ = m; }
        void setAlbedoMap(std::string path) { albedoMapPath_ = std::move(path); }

        [[nodiscard]] const Color& color()   const { return color_; }
        [[nodiscard]] float roughness()      const { return roughness_; }
        [[nodiscard]] float metallic()       const { return metallic_; }
        [[nodiscard]] bool  isLit()          const { return lit_; }
        [[nodiscard]] const std::string& albedoMapPath() const { return albedoMapPath_; }

        // Internal GPU state — set by engine, not user code
        void setAlbedoTextureId(uint32_t id) { albedoTexId_ = id; }
        [[nodiscard]] uint32_t albedoTextureId() const { return albedoTexId_; }
        [[nodiscard]] bool hasAlbedoTexture() const { return albedoTexId_ != 0; }

    private:
        explicit Material(bool lit);

        Color       color_{ 1.0f, 1.0f, 1.0f, 1.0f };
        float       roughness_     = 0.5f;
        float       metallic_      = 0.0f;
        bool        lit_           = true;
        std::string albedoMapPath_;
        uint32_t    albedoTexId_   = 0;
    };

} // namespace Nox

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
        void setNormalMap(std::string path) { normalMapPath_ = std::move(path); }
        void setEmissiveMap(std::string path) { emissiveMapPath_ = std::move(path); }
        void setEmissiveColor(const Color& c) { emissiveColor_ = c; }
        void setEmissiveIntensity(float i) { emissiveIntensity_ = i; }

        [[nodiscard]] const Color& color()   const { return color_; }
        [[nodiscard]] float roughness()      const { return roughness_; }
        [[nodiscard]] float metallic()       const { return metallic_; }
        [[nodiscard]] bool  isLit()          const { return lit_; }
        [[nodiscard]] const std::string& albedoMapPath() const { return albedoMapPath_; }
        [[nodiscard]] const std::string& normalMapPath() const { return normalMapPath_; }
        [[nodiscard]] const std::string& emissiveMapPath() const { return emissiveMapPath_; }
        [[nodiscard]] const Color& emissiveColor() const { return emissiveColor_; }
        [[nodiscard]] float emissiveIntensity() const { return emissiveIntensity_; }

        // Internal GPU state — set by engine, not user code
        void setAlbedoTextureId(uint32_t id) { albedoTexId_ = id; }
        [[nodiscard]] uint32_t albedoTextureId() const { return albedoTexId_; }
        [[nodiscard]] bool hasAlbedoTexture() const { return albedoTexId_ != 0; }

        void setNormalTextureId(uint32_t id) { normalTexId_ = id; }
        [[nodiscard]] uint32_t normalTextureId() const { return normalTexId_; }
        [[nodiscard]] bool hasNormalTexture() const { return normalTexId_ != 0; }

        void setEmissiveTextureId(uint32_t id) { emissiveTexId_ = id; }
        [[nodiscard]] uint32_t emissiveTextureId() const { return emissiveTexId_; }
        [[nodiscard]] bool hasEmissiveTexture() const { return emissiveTexId_ != 0; }

    private:
        explicit Material(bool lit);

        Color       color_{ 1.0f, 1.0f, 1.0f, 1.0f };
        float       roughness_     = 0.5f;
        float       metallic_      = 0.0f;
        bool        lit_           = true;
        std::string albedoMapPath_;
        std::string normalMapPath_;
        std::string emissiveMapPath_;
        Color       emissiveColor_{ 0.0f, 0.0f, 0.0f, 1.0f };
        float       emissiveIntensity_ = 0.0f;
        uint32_t    albedoTexId_   = 0;
        uint32_t    normalTexId_   = 0;
        uint32_t    emissiveTexId_ = 0;
    };

} // namespace Nox

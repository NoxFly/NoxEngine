// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/core/Color.hpp>
#include <NoxEngine/math/Types.hpp>
#include <NoxEngine/scene/SceneObject.hpp>

#include <memory>

namespace Nox {

    // ── Light base ─────────────────────────────────────────────────
    class Light : public SceneObject {
    public:
        Light(std::string name, const Color& color, float intensity);
        ~Light() override = default;

        void setColor(const Color& c)    { color_ = c; }
        void setIntensity(float i)       { intensity_ = i; }

        [[nodiscard]] const Color& color() const { return color_; }
        [[nodiscard]] float intensity()    const { return intensity_; }

    private:
        Color color_;
        float intensity_;
    };

    // ── Directional light ──────────────────────────────────────────
    class DirectionalLight : public Light {
    public:
        DirectionalLight(const Color& color = Color::White, float intensity = 1.0f);

        void setDirection(float x, float y, float z);
        void setDirection(const Math::Vec3& dir);
        [[nodiscard]] const Math::Vec3& direction() const { return direction_; }

    private:
        Math::Vec3 direction_{ -1.0f, -1.0f, -0.5f };
    };

    // ── Point light ────────────────────────────────────────────────
    class PointLight : public Light {
    public:
        PointLight(const Color& color = Color::White, float intensity = 1.0f);

        void setRange(float range) { range_ = range; }
        [[nodiscard]] float range() const { return range_; }

    private:
        float range_ = 10.0f;
    };

    // ── Ambient light ──────────────────────────────────────────────
    class AmbientLight : public Light {
    public:
        AmbientLight(const Color& color = Color::White, float intensity = 0.1f);
    };

} // namespace Nox

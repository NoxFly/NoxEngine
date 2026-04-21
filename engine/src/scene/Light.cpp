// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/scene/Light.hpp>

#include <glm/glm.hpp>

namespace Nox {

    Light::Light(std::string name, const Color& color, float intensity, SceneObjectType type)
        : SceneObject(std::move(name), type)
        , color_(color)
        , intensity_(intensity) {}

    // ── DirectionalLight ───────────────────────────────────────────
    DirectionalLight::DirectionalLight(const Color& color, float intensity)
        : Light("DirectionalLight", color, intensity, SceneObjectType::DirectionalLight) {}

    void DirectionalLight::setDirection(float x, float y, float z) {
        direction_ = glm::normalize(Math::Vec3(x, y, z));
    }

    void DirectionalLight::setDirection(const Math::Vec3& dir) {
        direction_ = glm::normalize(dir);
    }

    // ── PointLight ─────────────────────────────────────────────────
    PointLight::PointLight(const Color& color, float intensity)
        : Light("PointLight", color, intensity, SceneObjectType::PointLight) {}

    // ── AmbientLight ───────────────────────────────────────────────
    AmbientLight::AmbientLight(const Color& color, float intensity)
        : Light("AmbientLight", color, intensity, SceneObjectType::AmbientLight) {}

} // namespace Nox

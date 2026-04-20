// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/scene/Material.hpp>

namespace Nox {

    Material::Material(bool lit)
        : lit_(lit) {}

    std::shared_ptr<Material> Material::standard() {
        return std::shared_ptr<Material>(new Material(true));
    }

    std::shared_ptr<Material> Material::unlit() {
        return std::shared_ptr<Material>(new Material(false));
    }

} // namespace Nox

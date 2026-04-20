// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/scene/Mesh.hpp>

namespace Nox {

    Mesh::Mesh(std::shared_ptr<Geometry> geometry, std::shared_ptr<Material> material)
        : SceneObject("Mesh")
        , geometry_(std::move(geometry))
        , material_(std::move(material)) {}

} // namespace Nox

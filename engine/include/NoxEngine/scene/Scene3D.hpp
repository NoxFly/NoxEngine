// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/scene/Light.hpp>
#include <NoxEngine/scene/Mesh.hpp>
#include <NoxEngine/scene/SceneObject.hpp>

#include <memory>
#include <vector>

namespace Nox {

    class Scene3D {
    public:
        Scene3D() = default;

        void add(std::shared_ptr<SceneObject> object);

        [[nodiscard]] const std::vector<std::shared_ptr<Mesh>>&  meshes() const { return meshes_; }
        [[nodiscard]] const std::vector<std::shared_ptr<Light>>& lights() const { return lights_; }

    private:
        std::vector<std::shared_ptr<SceneObject>> objects_;
        std::vector<std::shared_ptr<Mesh>>        meshes_;
        std::vector<std::shared_ptr<Light>>       lights_;
    };

} // namespace Nox

// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/scene/Scene3D.hpp>

namespace Nox {

    void Scene3D::add(std::shared_ptr<SceneObject> object) {
        // Try to classify the object for fast iteration
        if (auto mesh = std::dynamic_pointer_cast<Mesh>(object)) {
            meshes_.push_back(mesh);
        }
        else if (auto light = std::dynamic_pointer_cast<Light>(object)) {
            lights_.push_back(light);
        }
        objects_.push_back(std::move(object));
    }

} // namespace Nox

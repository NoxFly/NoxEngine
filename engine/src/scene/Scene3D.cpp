// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/scene/Scene3D.hpp>

#include <algorithm>

namespace Nox {

    void Scene3D::add(std::shared_ptr<SceneObject> object) {
        if (auto mesh = std::dynamic_pointer_cast<Mesh>(object)) {
            meshes_.push_back(mesh);
        }
        else if (auto light = std::dynamic_pointer_cast<Light>(object)) {
            lights_.push_back(light);
        }
        objects_.push_back(std::move(object));
    }

    void Scene3D::remove(const std::shared_ptr<SceneObject>& object) {
        std::erase(objects_, object);

        if (auto mesh = std::dynamic_pointer_cast<Mesh>(object)) {
            std::erase(meshes_, mesh);
        }
        else if (auto light = std::dynamic_pointer_cast<Light>(object)) {
            std::erase(lights_, light);
        }
    }

    std::shared_ptr<SceneObject> Scene3D::findByName(std::string_view name) const {
        for (const auto& obj : objects_) {
            if (obj->name() == name) {
                return obj;
            }
        }
        return nullptr;
    }

} // namespace Nox

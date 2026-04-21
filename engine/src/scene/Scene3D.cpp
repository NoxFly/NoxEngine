// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/scene/Scene3D.hpp>
#include <NoxEngine/renderer/GPUInstancing.hpp>

#include <algorithm>

namespace Nox {

    void Scene3D::add(std::shared_ptr<SceneObject> object) {
        if (object->isMesh()) {
            meshes_.push_back(std::static_pointer_cast<Mesh>(object));
        }
        else if (object->isLight()) {
            lights_.push_back(std::static_pointer_cast<Light>(object));
        }
        objects_.push_back(std::move(object));
    }

    void Scene3D::remove(const std::shared_ptr<SceneObject>& object) {
        std::erase(objects_, object);

        if (object->isMesh()) {
            std::erase(meshes_, std::static_pointer_cast<Mesh>(object));
        }
        else if (object->isLight()) {
            std::erase(lights_, std::static_pointer_cast<Light>(object));
        }
    }

    std::shared_ptr<InstancedMesh> Scene3D::addInstanced(
        std::shared_ptr<Geometry> geometry,
        std::shared_ptr<Material> material,
        std::vector<Math::Mat4> transforms)
    {
        auto instanced = std::make_shared<InstancedMesh>(
            std::move(geometry), std::move(material), std::move(transforms));
        instancedMeshes_.push_back(instanced);
        objects_.push_back(instanced);
        return instanced;
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

// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/scene/Light.hpp>
#include <NoxEngine/scene/Mesh.hpp>
#include <NoxEngine/scene/SceneObject.hpp>

#include <functional>
#include <memory>
#include <string_view>
#include <vector>

namespace Nox {

    class Scene3D {
    public:
        Scene3D() = default;

        void add(std::shared_ptr<SceneObject> object);
        void remove(const std::shared_ptr<SceneObject>& object);

        [[nodiscard]] const std::vector<std::shared_ptr<Mesh>>&  meshes() const { return meshes_; }
        [[nodiscard]] const std::vector<std::shared_ptr<Light>>& lights() const { return lights_; }
        [[nodiscard]] const std::vector<std::shared_ptr<SceneObject>>& objects() const { return objects_; }

        // ── Traversal utilities ────────────────────────────────────
        [[nodiscard]] std::shared_ptr<SceneObject> findByName(std::string_view name) const;

        template<typename T>
        [[nodiscard]] std::vector<std::shared_ptr<T>> findAllOfType() const {
            std::vector<std::shared_ptr<T>> result;
            for (const auto& obj : objects_) {
                auto casted = std::dynamic_pointer_cast<T>(obj);
                if (casted) {
                    result.push_back(casted);
                }
            }
            return result;
        }

        template<typename T>
        void forEach(std::function<void(T&)> fn) {
            for (const auto& obj : objects_) {
                auto casted = std::dynamic_pointer_cast<T>(obj);
                if (casted) {
                    fn(*casted);
                }
            }
        }

    private:
        std::vector<std::shared_ptr<SceneObject>> objects_;
        std::vector<std::shared_ptr<Mesh>>        meshes_;
        std::vector<std::shared_ptr<Light>>       lights_;
    };

} // namespace Nox

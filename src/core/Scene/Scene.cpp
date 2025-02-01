/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Scene.hpp"

namespace NoxEngine {

    Scene::Scene():
        m_objects{}, m_lights{}
    {}

    Scene::~Scene()
    {}


    void Scene::add(std::shared_ptr<Actor> object) noexcept {
        m_objects.push_back(object);
    }

    void Scene::add(std::shared_ptr<Light> light) noexcept {
        m_lights.push_back(light);
    }

    std::vector<std::shared_ptr<Actor>> Scene::getActors() const noexcept {
        return m_objects;
    }

    std::vector<std::shared_ptr<Light>> Scene::getLights() const noexcept {
        return m_lights;
    }


}
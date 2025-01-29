/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <type_traits>
#include <glm/glm.hpp>
#include <memory>

#include "core/engine.typedef.hpp"
#include "core/Actor/Light/Light.hpp"

namespace NoxEngine {

    template <Dimension D>
    class Actor;

    template <Dimension D>
    class Scene {
        public:
            explicit Scene();
            Scene(Scene<D>&) = delete;
            ~Scene();

            Scene<D>& operator=(const Scene<D>&) = delete;

            void add(std::shared_ptr<Actor<D>> object) noexcept;
            void add(std::shared_ptr<Light> light) noexcept;

            std::vector<std::shared_ptr<Actor<D>>> getActors() const noexcept;
            std::vector<std::shared_ptr<Light>> getLights() const noexcept;

        protected:
            std::vector<std::shared_ptr<Actor<D>>> m_objects;
            std::vector<std::shared_ptr<Light>> m_lights;
    };

}

#include "./Scene.inl"

#endif // SCENE_HPP
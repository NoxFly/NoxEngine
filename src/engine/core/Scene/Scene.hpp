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

#include "engine/core/engine.typedef.hpp"
#include "engine/Rendering/Light/Light.hpp"

namespace NoxEngine {

    class Actor;

    class Scene {
        public:
            explicit Scene() = default;
            Scene(Scene&) = delete;
            ~Scene() = default;

            Scene& operator=(const Scene&) = delete;

            void add(std::shared_ptr<Actor> object) noexcept;
            void add(std::shared_ptr<Light> light) noexcept;

            std::vector<std::shared_ptr<Actor>> getActors() const noexcept;
            std::vector<std::shared_ptr<Light>> getLights() const noexcept;

        protected:
            std::vector<std::shared_ptr<Actor>> m_objects {};
            std::vector<std::shared_ptr<Light>> m_lights {};
    };

}

#endif // SCENE_HPP
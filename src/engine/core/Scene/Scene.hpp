#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <type_traits>
#include <glm/glm.hpp>

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

            void add(std::shared_ptr<Actor<D>> object);
            void add(std::shared_ptr<Light> light);

            std::vector<std::shared_ptr<Actor<D>>> getActors() const;
            std::vector<std::shared_ptr<Light>> getLights() const;

        protected:
            std::vector<std::shared_ptr<Actor<D>>> m_objects;
            std::vector<std::shared_ptr<Light>> m_lights;
    };

}

#include "./Scene.inl"

#endif // SCENE_HPP
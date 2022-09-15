#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <type_traits>
#include <glm/glm.hpp>

#include "engine.typedef.hpp"
#include "Actor.hpp"

namespace NoxEngine {

    template <Dimension D>
    class Scene {
        public:
            explicit Scene();
            Scene(Scene<D>&) = delete;
            ~Scene();

            Scene<D>& operator=(const Scene<D>&) = delete;

            void add(std::shared_ptr<Actor<D>> object);
            std::vector<std::shared_ptr<Actor<D>>> getActors() const;

        protected:
            std::vector<std::shared_ptr<Actor<D>>> m_objects;
    };

}

#include "Scene.inl"

#endif // SCENE_HPP
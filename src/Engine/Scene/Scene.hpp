#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>

#include "Object.hpp"

class Scene {
    public:
        Scene();
        ~Scene();

        void add(Object* object);
        std::vector<Object*> getObjects() const;

    private:
        std::vector<Object*> m_objects;
};

#endif // SCENE_HPP
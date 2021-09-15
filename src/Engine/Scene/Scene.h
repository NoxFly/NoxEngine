#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "Object.h"

class Scene {
    public:
        Scene();
        ~Scene();

        void add(const Object& object);

    private:
        std::vector<Object> m_objects;
};

#endif // SCENE_H
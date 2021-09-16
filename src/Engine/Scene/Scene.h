#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "Object.h"

class Scene {
    public:
        Scene();
        ~Scene();

        void add(Object* object);
        std::vector<Object*> getObjects() const;

    private:
        std::vector<Object*> m_objects;
};

#endif // SCENE_H
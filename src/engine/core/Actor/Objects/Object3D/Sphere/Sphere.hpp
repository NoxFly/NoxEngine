#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <string>
#include <glm/glm.hpp>
#include <memory>

#include "core/Actor/Objects/Object3D/Object3D.hpp"


namespace NoxEngine {

    class Sphere : public Object3D {
    public:
        explicit Sphere(const float radius);
        ~Sphere();

    protected:

    };

}

#endif // SPHERE_HPP
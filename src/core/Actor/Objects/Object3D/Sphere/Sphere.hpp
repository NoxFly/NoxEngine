/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

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
    };

}

#endif // SPHERE_HPP
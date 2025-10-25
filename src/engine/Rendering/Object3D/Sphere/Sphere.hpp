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

#include "engine/Rendering/Components/Mesh3D/Mesh3D.hpp"


namespace NoxEngine {

    class Sphere : public Mesh3D {
        public:
            explicit Sphere(const float radius);
            ~Sphere() = default;
    };

}

#endif // SPHERE_HPP
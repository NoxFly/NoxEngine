/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef SPHERE_GEOMETRY_HPP
#define SPHERE_GEOMETRY_HPP

#include <glm/glm.hpp>

#include "engine/core/engine.typedef.hpp"
#include "engine/Rendering/Geometries/BufferGeometry.hpp"


namespace NoxEngine {

    class SphereGeometry: public BufferGeometry {
        public:
            explicit SphereGeometry(const float radius, const unsigned short nbPointsH=70, const unsigned short nbPointsV=70);
            ~SphereGeometry() = default;
    };

}

#endif // SPHERE_GEOMETRY_HPP
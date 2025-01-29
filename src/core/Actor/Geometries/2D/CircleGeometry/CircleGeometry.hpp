/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef CIRCLE_GEOMETRY_HPP
#define CIRCLE_GEOMETRY_HPP

#include <glm/glm.hpp>

#include "core/engine.typedef.hpp"
#include "core/Actor/Geometries/BufferGeometry/BufferGeometry.hpp"


namespace NoxEngine {

    class CircleGeometry: public BufferGeometry<V2D> {
        public:
            explicit CircleGeometry(const float radius, const unsigned short nbPoints=40);
            ~CircleGeometry();
    };

}

#endif // CIRCLE_GEOMETRY_HPP
/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef BOX_GEOMETRY_HPP
#define BOX_GEOMETRY_HPP

#include <glm/glm.hpp>

#include "core/engine.typedef.hpp"
#include "core/Actor/Geometries/BufferGeometry/BufferGeometry.hpp"


namespace NoxEngine {

    class BoxGeometry: public BufferGeometry<V3D> {
        public:
            explicit BoxGeometry(const float width, const float height, const float depth);
            explicit BoxGeometry(const V3D& size);
            ~BoxGeometry();
    };

}

#endif // BOX_GEOMETRY_HPP
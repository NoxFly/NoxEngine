#ifndef RECTANGLE_GEOMETRY_HPP
#define RECTANGLE_GEOMETRY_HPP

#include <glm/glm.hpp>

#include "core/engine.typedef.hpp"
#include "core/Actor/Geometries/BufferGeometry/BufferGeometry.hpp"


namespace NoxEngine {

    class RectangleGeometry : public BufferGeometry<V2D> {
        public:
            explicit RectangleGeometry(const float width, const float height);
            explicit RectangleGeometry(const V2D& size);
            ~RectangleGeometry();
    };

}

#endif // RECTANGLE_GEOMETRY_HPP
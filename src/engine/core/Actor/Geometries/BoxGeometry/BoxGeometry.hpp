#ifndef BOX_GEOMETRY_HPP
#define BOX_GEOMETRY_HPP

#include <glm/glm.hpp>

#include "core/engine.typedef.hpp"
#include "core/Actor/Geometries/Geometry.hpp"


namespace NoxEngine {

    class BoxGeometry: public Geometry {
        public:
            explicit BoxGeometry(const float width, const float height, const float depth);
            explicit BoxGeometry(const V3D& size);
            ~BoxGeometry();
    };

}

#endif // BOX_GEOMETRY_HPP
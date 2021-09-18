#ifndef BOX_GEOMETRY_H
#define BOX_GEOMETRY_H

#include <glm/glm.hpp>

#include "Geometry.h"

class BoxGeometry: public Geometry {
    public:
        BoxGeometry(const float width, const float height, const float depth);
        BoxGeometry(const glm::vec3& size);
        ~BoxGeometry();
};

#endif // BOX_GEOMETRY_H
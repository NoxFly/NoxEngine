#ifndef BOX_GEOMETRY_HPP
#define BOX_GEOMETRY_HPP

#include <glm/glm.hpp>

#include "Geometry.hpp"

class BoxGeometry: public Geometry {
    public:
        BoxGeometry(const float width, const float height, const float depth);
        BoxGeometry(const glm::vec3& size);
        ~BoxGeometry();
};

#endif // BOX_GEOMETRY_HPP
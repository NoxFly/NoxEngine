#include "Circle.hpp"

#include "core/Actor/Geometries/2D/CircleGeometry/CircleGeometry.hpp"
#include "core/Actor/Materials/PhongMaterial/PhongMaterial.hpp"

namespace NoxEngine {

    Circle::Circle():
        Circle(0)
    {

    }

    Circle::Circle(float radius):
        Object2D(CircleGeometry(radius), PhongMaterial()),
        m_radius(radius)
    {
        
    }

    Circle::~Circle() {

    }


    float Circle::radius() const noexcept {
        return m_radius;
    }

}
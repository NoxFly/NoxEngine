#include "Circle.hpp"

#include "core/Actor/Geometries/2D/CircleGeometry/CircleGeometry.hpp"
#include "core/Actor/Materials/2D/Basic2DMaterial/Basic2DMaterial.hpp"


namespace NoxEngine {

    Circle::Circle():
        Circle(0)
    {}

    Circle::Circle(float radius):
        Object2D(new CircleGeometry(radius), new Basic2DMaterial()),
        m_radius(radius)
    {}

    Circle::~Circle() {}


    float Circle::radius() const noexcept {
        return m_radius;
    }

}
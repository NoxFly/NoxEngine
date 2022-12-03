#include "Circle.hpp"

namespace NoxEngine {

    Circle::Circle():
        Circle(0)
    {

    }

    Circle::Circle(float radius):
        Object2D(),
        m_radius(radius)
    {
        // load();
    }

    Circle::~Circle() {

    }


    float Circle::radius() const noexcept {
        return m_radius;
    }

}
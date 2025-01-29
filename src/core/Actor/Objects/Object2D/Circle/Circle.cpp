/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

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
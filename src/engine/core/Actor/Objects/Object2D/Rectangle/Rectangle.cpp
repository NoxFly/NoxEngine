#include "Rectangle.hpp"

#include "core/Actor/Geometries/2D/RectangleGeometry/RectangleGeometry.hpp"
#include "core/Actor/Materials/PhongMaterial/PhongMaterial.hpp"

namespace NoxEngine {

    Rectangle::Rectangle():
        Rectangle(2, 2)
    {}

    Rectangle::Rectangle(V2D& size):
        Rectangle(size.x, size.y)
    {

    }

    Rectangle::Rectangle(float width, float height):
        Object2D(RectangleGeometry(width, height), PhongMaterial()),
        m_size(width, height)
    {}

    Rectangle::~Rectangle() {

    }


    const V2D& Rectangle::size() noexcept {
        return m_size;
    }

}
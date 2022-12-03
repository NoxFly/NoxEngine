#include "Rectangle.hpp"


namespace NoxEngine {

    Rectangle::Rectangle():
        Rectangle(2, 2)
    {}

    Rectangle::Rectangle(V2D& size):
        Rectangle(size.x, size.y)
    {

    }

    Rectangle::Rectangle(float width, float height):
        Object2D(),
        m_size(width, height)
    {
        // load();
    }

    Rectangle::~Rectangle() {

    }


    const V2D& Rectangle::size() noexcept {
        return m_size;
    }

}
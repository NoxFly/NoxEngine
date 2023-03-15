#include "Matrices2D.hpp"

namespace NoxEngine {

    Matrices2D::Matrices2D():
        Matrices(M3(1.0f))
    {}

    Matrices2D::~Matrices2D()
    {}

    void Matrices2D::translate(const V2D& translation) noexcept {
        Matrices::_translate(V3D(translation, 0.f));
    }

    void Matrices2D::rotate(const V2D& rotation) noexcept {
        Matrices::_rotate(V3D(rotation, 0.f));
    }

}
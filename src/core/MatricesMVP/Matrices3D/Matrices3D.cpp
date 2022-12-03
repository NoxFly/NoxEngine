#include "Matrices3D.hpp"

#include <iostream>

#include "utils.hpp"

namespace NoxEngine {

    Matrices3D::Matrices3D(double fov, double aspect, double near, double far, const V3D& position, const V3D& verticalAxis):
        Matrices(glm::lookAt(position, V3D(0, 0, 0), V3D(0, 1, 0))),
        m_projection(glm::perspective(fov, aspect, near, far))
    {
        std::cout << "Init M3D with :"
            << "\n  fov : " << fov
            << "\n  aspect : " << aspect
            << "\n  near : " << near
            << "\n  far : " << far
            << "\n  position : " << vecToString(position)
            << "\n  verticalAxis : " << vecToString(verticalAxis)
            << std::endl;
    }

    Matrices3D::~Matrices3D()
    {}

    Matrices3D::Matrices3D(const Matrices3D& copy):
        Matrices(copy.m_view, copy.m_saves),
        m_projection(copy.m_projection)
    {}

    Matrices3D& Matrices3D::operator=(const Matrices3D& copy) {
        if(this != &copy) {
            m_projection = copy.getProjection();
            m_model = copy.getModel();
            m_view = copy.getView();
            m_saves = {};
            m_needsToUpdate = true;
        }

        return *this;
    }

    void Matrices3D::update() noexcept {
        m_mvp = m_projection * m_model * m_view;
        m_needsToUpdate = false;
    }

    void Matrices3D::translate(const V3D& translation) noexcept {
        Matrices::_translate(translation);
    }

    void Matrices3D::rotate(const V3D& rotation) noexcept {
        Matrices::_rotate(rotation);
    }

    M4 Matrices3D::getProjection() const noexcept {
        return m_projection;
    }

    M4& Matrices3D::getProjection() noexcept {
        return m_projection;
    }

}
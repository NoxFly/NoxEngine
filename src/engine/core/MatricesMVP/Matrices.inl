#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

#include "Console/Console.hpp"
#include "Matrices.hpp"


namespace NoxEngine {

    template <Dimension D>
    Matrices<D>::Matrices():
        Matrices(glm::lookAt(V3D(0, 0, 0), V3D(0, 0, 0), V3D(0, 1, 0)), {})
    {}

    template <Dimension D>
    Matrices<D>::Matrices(M4 view):
        Matrices(view, {})
    {}

    template <Dimension D>
    Matrices<D>::Matrices(M4 view, std::stack<M4> saves):
        m_needsToUpdate(true),
        m_model(1.0f),
        m_view(view),
        m_projection(1.0f),
        m_mvp(),
        m_saves{saves}
    {}

    template <Dimension D>
    Matrices<D>::Matrices(float left, float right, float top, float bottom, float near, float far, const V3D& position, const V3D& verticalAxis) requires Is3D<D>:
        Matrices(glm::lookAt(position, V3D(0, 0, 0), V3D(0, 1, 0)))
    {
        m_projection = glm::ortho(left, right, bottom, right, near, far);
    }

    template <Dimension D>
    Matrices<D>::Matrices(float fov, float aspect, float near, float far, const V3D& position, const V3D& verticalAxis) requires Is3D<D>:
        Matrices(glm::lookAt(position, V3D(0, 0, 0), V3D(0, 1, 0)))
    {
            m_projection = glm::perspective(fov, aspect, near, far);
    }


    template <Dimension D>
    void Matrices<D>::push() noexcept {
        m_saves.push(m_model);
    }

    template <Dimension D>
    void Matrices<D>::pop() noexcept {
        if(!m_saves.empty()) {
            m_model = m_saves.top();
            m_saves.pop();
            m_needsToUpdate = true;
        }
    }

    template <Dimension D>
    M4 Matrices<D>::getModel() const noexcept {
        return m_model;
    }

    template <Dimension D>
    M4& Matrices<D>::getModel() noexcept {
        return m_model;
    }

    template <Dimension D>
    M4 Matrices<D>::getView() const noexcept {
        return m_view;
    }

    template <Dimension D>
    M4& Matrices<D>::getView() noexcept {
        return m_view;
    }

    template <Dimension D>
    M4 Matrices<D>::getProjection() const noexcept requires Is3D<D> {
        return m_projection;
    }

    template <Dimension D>
    M4& Matrices<D>::getProjection() noexcept requires Is3D<D> {
        return m_projection;
    }

    template <Dimension D>
    M4& Matrices<D>::get() noexcept {
        if(m_needsToUpdate)
            update();

        return m_mvp;
    }

    template <Dimension D>
    void Matrices<D>::setView(const M4& lookAt) noexcept {
        m_view = lookAt;
        m_needsToUpdate = true;
    }

    template <Dimension D>
    void Matrices<D>::translate(const V3D& translation) noexcept requires Is3D<D> {
        _translate(translation);
    }

    template <Dimension D>
    void Matrices<D>::rotate(const V3D& rotation) noexcept requires Is3D<D> {
        _rotate(rotation);
    }

    template <Dimension D>
    void Matrices<D>::translate(const V2D& translation) noexcept requires Is2D<D> {
        _translate(V3D(translation, 0.f));
    }
    
    template <Dimension D>
    void Matrices<D>::rotate(const V2D& rotation) noexcept requires Is2D<D> {
        _rotate(V3D(rotation, 0.f));
    }

    template <Dimension D>
    void Matrices<D>::_translate(const V3D& translation) noexcept {
        m_model = glm::translate(m_model, translation);
        m_needsToUpdate = true;
    }
    

    template <Dimension D>
    void Matrices<D>::_rotate(const V3D& rotation) noexcept {
        if(rotation.x != 0)
            m_model = glm::rotate(m_model, glm::radians(rotation.x), V3D(0, 1, 0));

        if(rotation.y != 0)
            m_model = glm::rotate(m_model, glm::radians(rotation.y), V3D(0, 0, 1));

        if(rotation.z != 0)
            m_model = glm::rotate(m_model, glm::radians(rotation.z), V3D(1, 0, 0));

        m_needsToUpdate = true;
    }

    template <Dimension D>
    void Matrices<D>::update() noexcept {
        m_mvp = m_projection * m_view * m_model;
        m_needsToUpdate = false;
    }

}
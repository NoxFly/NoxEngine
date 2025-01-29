/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

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
    Matrices<D>::Matrices(const M4& view):
        Matrices(view, {})
    {}

    template <Dimension D>
    Matrices<D>::Matrices(const M4& view, const std::stack<M4>& saves):
        m_needsUpdate(true),
        m_model(1.0f),
        m_view(view),
        m_mvp(),
        m_projection(1.0f),
        m_saves{saves}
    {}

    template <Dimension D>
    Matrices<D>::Matrices(const float left, const float right, const float top, const float bottom, const float near, const float far, const V3D& position, const V3D& verticalAxis) requires Is3D<D>:
        Matrices(glm::lookAt(position, V3D(0, 0, 0), verticalAxis))
    {
        m_projection = glm::ortho(left, right, bottom, top, near, far);
    }

    template <Dimension D>
    Matrices<D>::Matrices(const float fov, const float aspect, const float near, const float far, const V3D& position, const V3D& verticalAxis) requires Is3D<D>:
        Matrices(glm::lookAt(position, V3D(0, 0, 0), verticalAxis))
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
            m_needsUpdate = true;
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
        if(m_needsUpdate)
            update();

        return m_mvp;
    }

    template <Dimension D>
    void Matrices<D>::setView(const M4& lookAt) noexcept {
        m_view = lookAt;
        m_needsUpdate = true;
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
    void Matrices<D>::scale(const V3D& scale) noexcept requires Is3D<D> {
        _scale(scale);
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
    void Matrices<D>::scale(const V2D& scale) noexcept requires Is2D<D> {
        _scale(V3D(scale, 1.f));
    }

    template <Dimension D>
    void Matrices<D>::_translate(const V3D& translation) noexcept {
        m_model = glm::translate(m_model, translation);
        m_needsUpdate = true;
    }
    

    template <Dimension D>
    void Matrices<D>::_rotate(const V3D& rotation) noexcept {
        if(rotation.x != 0)
            m_model = glm::rotate(m_model, glm::radians(rotation.x), V3D(0, 1, 0));

        if(rotation.y != 0)
            m_model = glm::rotate(m_model, glm::radians(rotation.y), V3D(0, 0, 1));

        if(rotation.z != 0)
            m_model = glm::rotate(m_model, glm::radians(rotation.z), V3D(1, 0, 0));

        m_needsUpdate = true;
    }

    template <Dimension D>
    void Matrices<D>::_scale(const V3D& scale) noexcept {
        m_model = glm::scale(m_model, scale);
        m_needsUpdate = true;
    }

    template <Dimension D>
    void Matrices<D>::update() noexcept {
        m_mvp = m_projection * m_view * m_model;
        m_needsUpdate = false;
    }

}
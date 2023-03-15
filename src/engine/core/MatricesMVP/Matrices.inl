#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

#include "Console/Console.hpp"

namespace NoxEngine {

    template <Dimension D>
    Matrices<D>::Matrices():
        Matrices(glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)), {})
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
        m_mvp(),
        m_saves{saves}
    {}


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
        m_mvp = m_model * m_view;
        m_needsToUpdate = false;
    }

}
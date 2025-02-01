/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Matrices.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

#include "Console/Console.hpp"
#include "Matrices.hpp"


namespace NoxEngine {

    Matrices::Matrices():
        Matrices(glm::lookAt(V3D(0, 0, 0), V3D(0, 0, 0), V3D(0, 1, 0)), {})
    {}

    Matrices::Matrices(const M4& view):
        Matrices(view, {})
    {}

    Matrices::Matrices(const M4& view, const std::stack<M4>& saves):
        m_needsUpdate(true),
        m_model(1.0f),
        m_view(view),
        m_mvp(),
        m_projection(1.0f),
        m_saves{saves}
    {}

    Matrices::Matrices(const float left, const float right, const float top, const float bottom, const float near, const float far, const V3D& position, const V3D& verticalAxis):
        Matrices(glm::lookAt(position, V3D(0, 0, 0), verticalAxis))
    {
        m_projection = glm::ortho(left, right, bottom, top, near, far);
    }

    Matrices::Matrices(const float fov, const float aspect, const float near, const float far, const V3D& position, const V3D& verticalAxis):
        Matrices(glm::lookAt(position, V3D(0, 0, 0), verticalAxis))
    {
        m_projection = glm::perspective(fov, aspect, near, far);
    }


    void Matrices::push() noexcept {
        m_saves.push(m_model);
    }

    void Matrices::pop() noexcept {
        if(!m_saves.empty()) {
            m_model = m_saves.top();
            m_saves.pop();
            m_needsUpdate = true;
        }
    }

    M4 Matrices::getModel() const noexcept {
        return m_model;
    }

    M4& Matrices::getModel() noexcept {
        return m_model;
    }

    M4 Matrices::getView() const noexcept {
        return m_view;
    }

    M4& Matrices::getView() noexcept {
        return m_view;
    }

    M4 Matrices::getProjection() const noexcept {
        return m_projection;
    }

    M4& Matrices::getProjection() noexcept {
        return m_projection;
    }

    M4& Matrices::get() noexcept {
        if(m_needsUpdate)
            update();

        return m_mvp;
    }

    void Matrices::setView(const M4& lookAt) noexcept {
        m_view = lookAt;
        m_needsUpdate = true;
    }

    void Matrices::translate(const V3D& translation) noexcept {
        _translate(translation);
    }

    void Matrices::rotate(const V3D& rotation) noexcept {
        _rotate(rotation);
    }

    void Matrices::scale(const V3D& scale) noexcept {
        _scale(scale);
    }

    void Matrices::_translate(const V3D& translation) noexcept {
        m_model = glm::translate(m_model, translation);
        m_needsUpdate = true;
    }
    

    void Matrices::_rotate(const V3D& rotation) noexcept {
        if(rotation.x != 0)
            m_model = glm::rotate(m_model, glm::radians(rotation.x), V3D(0, 1, 0));

        if(rotation.y != 0)
            m_model = glm::rotate(m_model, glm::radians(rotation.y), V3D(0, 0, 1));

        if(rotation.z != 0)
            m_model = glm::rotate(m_model, glm::radians(rotation.z), V3D(1, 0, 0));

        m_needsUpdate = true;
    }

    void Matrices::_scale(const V3D& scale) noexcept {
        m_model = glm::scale(m_model, scale);
        m_needsUpdate = true;
    }

    void Matrices::update() noexcept {
        m_mvp = m_projection * m_view * m_model;
        m_needsUpdate = false;
    }

}
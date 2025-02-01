/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "PerspectiveCamera.hpp"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <algorithm>


namespace NoxEngine {

    PerspectiveCamera::PerspectiveCamera(const float fov, const float aspect, const float near, const float far):
        Camera(fov, aspect, near, far),
        m_orientation()
    {}

    // Set's the camera's look at. The view matrix will be upated in the loop
    void PerspectiveCamera::lookAt(const V3D eye, const V3D target, const V3D up) noexcept {
        Camera::_setPosition(eye);
        m_verticalAxis = up;
        lookAt(target);
    }

    void PerspectiveCamera::lookAt(const V3D& target) noexcept {
        m_target = target;
        V3D direction = glm::normalize(m_target - m_position);
        m_orientation = glm::quatLookAt(direction, m_verticalAxis);
        m_needsUpdate = true;
    }

    void PerspectiveCamera::lookAt(const float x, const float y, const float z) noexcept {
        lookAt(V3D(x, y, z));
    }

    void PerspectiveCamera::move(const V3D& offset, const unsigned int duration) {
        if(duration == 0) {
            // move in the direction of the quaternion
            m_position += glm::rotate(m_orientation, offset);
            m_target = m_position + getForward();
            m_needsUpdate = true;
        }
    }

    void PerspectiveCamera::moveTo(const V3D& position, const unsigned int duration) {
        if(duration == 0) {
            m_position = position;
            m_target = m_position + getForward();
            m_needsUpdate = true;
        }
    }

    void PerspectiveCamera::setOrientation(const glm::quat& orientation) noexcept {
        m_orientation = orientation;
        m_target = m_position + getForward();
        m_needsUpdate = true;
    }

    /**
     * Rotate from Euler angles (in radians)
     */
    void PerspectiveCamera::orientate(const V3D& offset) noexcept {
        glm::quat qPitch = glm::angleAxis(offset.x, V3D(1, 0, 0));
        glm::quat qYaw = glm::angleAxis(offset.y, V3D(0, 1, 0));
        glm::quat qRoll = glm::angleAxis(offset.z, V3D(0, 0, 1));

        m_orientation = glm::normalize(qYaw * qPitch * qRoll * m_orientation);
        m_needsUpdate = true;
    }

    glm::quat PerspectiveCamera::getOrientation() const noexcept {
        return m_orientation;
    }

    V3D PerspectiveCamera::getForward() const noexcept {
        return glm::rotate(m_orientation, V3D(0.0f, 0.0f, -1.0f));
    }

    V3D PerspectiveCamera::getRight() const noexcept {
        return glm::rotate(m_orientation, V3D(1.0f, 0.0f, 0.0f));
    }

    V3D PerspectiveCamera::getUp() const noexcept {
        return glm::rotate(m_orientation, V3D(0.0f, 1.0f, 0.0f));
    }

    void PerspectiveCamera::update() noexcept {
        if(m_needsUpdate) {
            m_needsUpdate = false;
            
            M4 rotate = glm::mat4_cast(m_orientation);
            M4 translate = M4(1.0f);
            translate = glm::translate(translate, -m_position);

            M4 view = rotate * translate;

            m_matrix.setView(view);
        }
    }

}
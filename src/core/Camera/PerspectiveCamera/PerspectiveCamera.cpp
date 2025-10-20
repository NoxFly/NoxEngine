/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "PerspectiveCamera.hpp"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <algorithm>


namespace NoxEngine {

    PerspectiveCamera::PerspectiveCamera(const float fov, const float aspect, const float near, const float far):
        Camera(fov, aspect, near, far),
        m_orientation(),
        m_pitch(0.0f),
        m_yaw(0.0f)
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
        
        // Extract pitch and yaw from the quaternion to keep them in sync
        extractPitchYawFromOrientation();
        
        m_dirty = true;
    }

    void PerspectiveCamera::lookAt(const float x, const float y, const float z) noexcept {
        lookAt(V3D(x, y, z));
    }

    void PerspectiveCamera::move(const V3D& offset, const unsigned int duration) {
        if(duration == 0) {
            // move in the direction of the quaternion
            m_position += glm::rotate(m_orientation, offset);
            m_target = m_position + getForward();
            m_dirty = true;
        }
    }

    void PerspectiveCamera::moveTo(const V3D& position, const unsigned int duration) {
        if(duration == 0) {
            m_position = position;
            m_target = m_position + getForward();
            m_dirty = true;
        }
    }

    void PerspectiveCamera::setOrientation(const glm::quat& orientation) noexcept {
        m_orientation = orientation;
        m_target = m_position + getForward();
        
        // Extract pitch and yaw from the quaternion to keep them in sync
        extractPitchYawFromOrientation();
        
        m_dirty = true;
    }

    /**
     * Rotate from Euler angles (in radians)
     */
    void PerspectiveCamera::orientate(const V3D& offset) noexcept {
        // For FPS controls without drift, we store pitch and yaw as angles
        // and reconstruct the quaternion each time
        
        // Update yaw (rotation around global Y axis)
        m_yaw += offset.y;
        
        // Update pitch (rotation around local X axis) and clamp to avoid gimbal lock
        m_pitch += offset.x;
        m_pitch = glm::clamp(m_pitch, -glm::half_pi<float>() + 0.01f, glm::half_pi<float>() - 0.01f);
        
        // Reconstruct orientation from pitch and yaw
        // For proper FPS controls: pitch * yaw (pitch in local space, yaw in global)
        glm::quat qYaw = glm::angleAxis(m_yaw, V3D(0, 1, 0));
        glm::quat qPitch = glm::angleAxis(m_pitch, V3D(1, 0, 0));
        
        // Combine: pitch first, then yaw (reverse order for quaternion multiplication)
        m_orientation = glm::normalize(qPitch * qYaw);
        
        m_dirty = true;
    }

    const V3D& PerspectiveCamera::getPosition() const noexcept {
        return m_position;
    }

    const glm::quat& PerspectiveCamera::getOrientation() const noexcept {
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

    void PerspectiveCamera::extractPitchYawFromOrientation() noexcept {
        // Extract Euler angles from quaternion
        // We need to extract pitch and yaw to keep them synchronized with m_orientation
        
        // Convert quaternion to Euler angles
        glm::vec3 euler = glm::eulerAngles(m_orientation);
        
        // euler.x = pitch (rotation around X axis)
        // euler.y = yaw (rotation around Y axis)
        // euler.z = roll (rotation around Z axis)
        
        m_pitch = euler.x;
        m_yaw = euler.y;
    }

    void PerspectiveCamera::update() noexcept {
        if(m_dirty) {
            m_dirty = false;
            
            M4 rotate = glm::mat4_cast(m_orientation);
            M4 translate = M4(1.0f);
            translate = glm::translate(translate, -m_position);

            M4 view = rotate * translate;

            m_matrix.setView(view);
        }
    }

}
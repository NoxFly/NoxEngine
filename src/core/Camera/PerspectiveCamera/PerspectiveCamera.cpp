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

#include "utils/quat.hpp"


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
        V3D dir = glm::normalize(target - m_position);

        if (glm::length2(dir) < 1e-8f) {
            m_dirty = true;
            return;
        }

        // yaw : orientation horizontale
        m_yaw = std::atan2(dir.x, -dir.z);

        // pitch : orientation verticale
        m_pitch = std::atan2(dir.y, std::sqrt(dir.x * dir.x + dir.z * dir.z));
        m_pitch = glm::clamp(m_pitch, -glm::half_pi<float>() + 0.01f, glm::half_pi<float>() - 0.01f);

        // reconstruction quaternion cohérente
        glm::quat qYaw   = glm::angleAxis(m_yaw, V3D(0.0f, 1.0f, 0.0f));
        glm::quat qPitch = glm::angleAxis(m_pitch, V3D(1.0f, 0.0f, 0.0f));
        
        m_orientation = glm::normalize(qYaw * qPitch);
        m_target = m_position + getForward();

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
        glm::quat qYaw = glm::angleAxis(m_yaw, V3D(0.0f, 1.0f, 0.0f));
        glm::quat qPitch = glm::angleAxis(m_pitch, V3D(1.0f, 0.0f, 0.0f));
        
        m_orientation = glm::normalize(qYaw * qPitch);
        m_target = m_position + getForward();
        
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

    void PerspectiveCamera::update() noexcept {
        if(m_dirty) {
            m_dirty = false;
            
            M4 rotate = glm::mat4_cast(glm::conjugate(m_orientation));
            M4 translate = glm::translate(M4(1.0f), -m_position);

            M4 view = rotate * translate;

            m_matrix.setView(view);
        }
    }

}
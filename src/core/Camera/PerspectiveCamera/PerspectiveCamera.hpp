/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef PERSPECTIVE_CAMERA_HPP
#define PERSPECTIVE_CAMERA_HPP

#include <glm/glm.hpp>

#include "core/engine.typedef.hpp"
#include "core/Camera/Camera.hpp"

/*

The orientation of the camera is represented by a quaternion.
a quaternion is [x, y, z, w] where x, y, z are the axis of rotation and w is the angle of rotation.

pitch : rotation around the camera's x-axis - like a flip/backflip
yaw : rotation around the camera's y-axis - like a spin
roll : rotation around the camera's z-axis - to "return" an object by the side


*/


namespace NoxEngine {

    class PerspectiveCamera: public Camera {
        public:
            explicit PerspectiveCamera(const float fov, const float aspect, const float near, const float far);

            void lookAt(const V3D eye, const V3D target, const V3D up) noexcept;
            void lookAt(const float x, const float y, const float z) noexcept;
            void lookAt(const V3D& target) noexcept;

            void move(const V3D& offset, const unsigned int duration = 0);
            void moveTo(const V3D& position, const unsigned int duration = 0);

            const V3D& getPosition() const noexcept;
            const glm::quat& getOrientation() const noexcept;

            void orientate(const V3D& offset) noexcept;

            V3D getForward() const noexcept;
            V3D getRight() const noexcept;
            V3D getUp() const noexcept;

            void update() noexcept override;

        private:
            glm::quat m_orientation;
            float m_pitch; // Rotation around X axis (up/down) - stored as angle for FPS controls
            float m_yaw;   // Rotation around Y axis (left/right) - stored as angle for FPS controls
    };

}

#endif // PERSPECTIVE_CAMERA_HPP
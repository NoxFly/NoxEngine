/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef PERSPECTIVE_CAMERA_HPP
#define PERSPECTIVE_CAMERA_HPP

#include <glm/glm.hpp>

#include "core/engine.typedef.hpp"
#include "core/Camera/3D/Camera3D.hpp"

/*

The orientation of the camera is represented by a quaternion.
a quaternion is [x, y, z, w] where x, y, z are the axis of rotation and w is the angle of rotation.

roll : rotation around the camera's z-axis - to "return" an object by the side
pitch : rotation around the camera's x-axis - like a flip/backflip
yaw : rotation around the camera's y-axis - like a spin


*/


namespace NoxEngine {

    class PerspectiveCamera: public Camera3D {
        public:
            explicit PerspectiveCamera(const float fov, const float aspect, const float near, const float far);

            void lookAt(const V3D eye, const V3D target, const V3D up) noexcept;
            void lookAt(const float x, const float y, const float z) noexcept;
            void lookAt(const V3D& target) noexcept;

            void move(const V3D& offset, const unsigned int duration = 0);
            void moveTo(const V3D& position, const unsigned int duration = 0);

            glm::quat getOrientation() const noexcept;
            void setOrientation(const glm::quat& orientation) noexcept;
            void orientate(const V3D& offset) noexcept;

            V3D getForward() const noexcept;
            V3D getRight() const noexcept;
            V3D getUp() const noexcept;

            void update() noexcept override;

        private:
            glm::quat m_orientation;
    };

}

#endif // PERSPECTIVE_CAMERA_HPP
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

            V3D getForward() const noexcept;
            V3D getRight() const noexcept;
            V3D getUp() const noexcept;

            void update() noexcept override;

        private:
            glm::quat m_orientation;
    };

}

#endif // PERSPECTIVE_CAMERA_HPP
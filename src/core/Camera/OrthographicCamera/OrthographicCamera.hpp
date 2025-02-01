/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef ORTHOGRAPHIC_CAMERA_HPP
#define ORTHOGRAPHIC_CAMERA_HPP

#include <glm/glm.hpp>

#include "core/engine.typedef.hpp"
#include "core/Camera/Camera.hpp"


namespace NoxEngine {

    class OrthographicCamera : public Camera {
        public:
            explicit OrthographicCamera(const float left, const float right, const float top, const float bottom);
            explicit OrthographicCamera(const float left, const float right, const float top, const float bottom, const float near, const float far);

            void move(const V3D& offset, const unsigned int duration = 0);
            void moveTo(const V3D& position, const unsigned int duration = 0);
    };

}

#endif // ORTHOGRAPHIC_CAMERA_HPP
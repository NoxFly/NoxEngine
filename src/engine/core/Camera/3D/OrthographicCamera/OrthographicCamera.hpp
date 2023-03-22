#ifndef ORTHOGRAPHIC_CAMERA_HPP
#define ORTHOGRAPHIC_CAMERA_HPP

#include <glm/glm.hpp>

#include "core/engine.typedef.hpp"
#include "core/Camera/3D/Camera3D.hpp"


namespace NoxEngine {

    class OrthographicCamera : public Camera3D {
        public:
            OrthographicCamera(float left, float right, float top, float bottom);
            OrthographicCamera(float left, float right, float top, float bottom, float near, float far);
            ~OrthographicCamera();

            void move(const V3D& offset, unsigned int duration = 0);
            void moveTo(const V3D& position, unsigned int duration = 0);
    };

}

#endif // ORTHOGRAPHIC_CAMERA_HPP
#ifndef PERSPECTIVE_CAMERA_HPP
#define PERSPECTIVE_CAMERA_HPP

#include <glm/glm.hpp>

#include "core/engine.typedef.hpp"
#include "core/Camera/3D/Camera3D.hpp"


namespace NoxEngine {

    class PerspectiveCamera: public Camera3D {
        public:
            explicit PerspectiveCamera(const float fov, const float aspect, const float near, const float far);
            ~PerspectiveCamera();

            void lookAt(const V3D eye, const V3D target, const V3D up) noexcept;
            void lookAt(const float x, const float y, const float z) noexcept;
            void lookAt(const V3D& target) noexcept;

            void orientate(const V2D& dir) noexcept;

            void move(const V3D& offset, const unsigned int duration = 0);
            void moveTo(const V3D& position, const unsigned int duration = 0);
    };

}

#endif // PERSPECTIVE_CAMERA_HPP
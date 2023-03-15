#ifndef PERSPECTIVE_CAMERA_HPP
#define PERSPECTIVE_CAMERA_HPP

#include <glm/glm.hpp>

#include "core/engine.typedef.hpp"
#include "core/Camera/3D/Camera3D.hpp"


namespace NoxEngine {

    class PerspectiveCamera: public Camera3D {
        public:
            PerspectiveCamera(double fov, double aspect, double near, double far);
            ~PerspectiveCamera();

            void lookAt(const V3D eye, const V3D target, const V3D up);
            void lookAt(const float x, const float y, const float z);
            void lookAt(const V3D& target);

            void orientate(const V2D& dir);

            void move(const V3D& offset, unsigned int duration = 0);
            void moveTo(const V3D& position, unsigned int duration = 0);

        private:
            V3D m_target;
    };

}

#endif // PERSPECTIVE_CAMERA_HPP
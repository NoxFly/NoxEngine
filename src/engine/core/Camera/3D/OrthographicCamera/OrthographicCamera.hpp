#ifndef ORTHOGRAPHIC_CAMERA_HPP
#define ORTHOGRAPHIC_CAMERA_HPP

#include <glm/glm.hpp>

#include "core/engine.typedef.hpp"
#include "core/Camera/3D/Camera3D.hpp"


namespace NoxEngine {

    class OrthographicCamera : public Camera3D {
        public:
            OrthographicCamera(double fov, double aspect, double near, double far);
            ~OrthographicCamera();

        protected:
            glm::mat4 ortho;
    };

}

#endif // ORTHOGRAPHIC_CAMERA_HPP
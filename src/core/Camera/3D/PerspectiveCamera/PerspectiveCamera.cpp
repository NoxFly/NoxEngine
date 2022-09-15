#include "PerspectiveCamera.hpp"

#include <glm/gtx/transform.hpp>
#include <algorithm>


namespace NoxEngine {

    PerspectiveCamera::PerspectiveCamera(double fov, double aspect, double near, double far):
        Camera3D(fov, aspect, near, far),
        m_target()
    {}

    PerspectiveCamera::~PerspectiveCamera()
    {}


    // Set's the camera's look at. The view matrix will be upated in the loop
    void PerspectiveCamera::lookAt(const V3D eye, const V3D target, const V3D up) {
        Camera::_setPosition(eye);
        lookAt(target);
        m_verticalAxis = up;
    }

    void PerspectiveCamera::lookAt(const V3D& target) {
        m_target = target;
        m_needUpdate = true;
    }

    void PerspectiveCamera::lookAt(const float x, const float y, const float z) {
        lookAt(V3D(x, y, z));
    }

    void PerspectiveCamera::move(const V3D& offset, uint duration) {
        if(duration == 0) {
            Camera3D::setPosition(m_position + offset);
        }
    }

    void PerspectiveCamera::moveTo(const V3D& position, uint duration) {
        if(duration == 0) {
            Camera3D::setPosition(position);
        }
    }

}
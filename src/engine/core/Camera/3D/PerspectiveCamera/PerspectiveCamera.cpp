#include "PerspectiveCamera.hpp"

#include <glm/gtx/transform.hpp>
#include <algorithm>


namespace NoxEngine {

    PerspectiveCamera::PerspectiveCamera(const float fov, const float aspect, const float near, const float far):
        Camera3D(fov, aspect, near, far)
    {}

    PerspectiveCamera::~PerspectiveCamera()
    {}


    // Set's the camera's look at. The view matrix will be upated in the loop
    void PerspectiveCamera::lookAt(const V3D eye, const V3D target, const V3D up) noexcept {
        Camera::_setPosition(eye);
        lookAt(target);
        m_verticalAxis = up;
    }

    void PerspectiveCamera::lookAt(const V3D& target) noexcept {
        m_target = target;
        m_needsUpdate = true;
    }

    void PerspectiveCamera::lookAt(const float x, const float y, const float z) noexcept {
        lookAt(V3D(x, y, z));
    }

    void PerspectiveCamera::move(const V3D& offset, const unsigned int duration) {
        if(duration == 0) {
            Camera3D::setPosition(m_position + offset);
        }
    }

    void PerspectiveCamera::moveTo(const V3D& position, const unsigned int duration) {
        if(duration == 0) {
            Camera3D::setPosition(position);
        }
    }

    void PerspectiveCamera::orientate(const V2D& dir) noexcept {
        // TODO
    }

}
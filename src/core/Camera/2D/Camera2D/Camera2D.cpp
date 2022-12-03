#include "Camera2D.hpp"

namespace NoxEngine {

    Camera2D::Camera2D():
        Camera(V3D(0, 0, 0), V3D(0, 0, 1), V3D(0, 1, 0))
    {}

    Camera2D::~Camera2D()
    {}

    V2D Camera2D::getPosition() const {
        return V2D(m_position.x, m_position.y);
    }

    float Camera2D::getZoom() const {
        return m_position.z;
    }

    void Camera2D::setPosition(const float x, const float y) {
        Camera::_setPosition(x, y, m_position.z);
    }

    void Camera2D::setPosition(const V2D& position) {
        Camera::_setPosition(position.x, position.y, m_position.z);
    }

    void Camera2D::zoomIn(float zoom, uint duration) {
        Camera::_setPosition(m_position.x, m_position.y, m_position.z + zoom);
        (void)duration;
    }

    void Camera2D::zoomOut(float zoom, uint duration) {
        Camera::_setPosition(m_position.x, m_position.y, m_position.z - zoom);
        (void)duration;
    }

    void Camera2D::setZoom(float zoom, uint duration) {
        Camera::_setPosition(m_position.x, m_position.y, zoom);
        (void)duration;
    }

    void Camera2D::move(const V2D& offset, uint duration) {
        if(duration == 0) {
            setPosition(offset);
        }
    }

    void Camera2D::moveTo(const V2D& position, uint duration) {
        if(duration == 0) {
            setPosition(position);
        }
    }

}
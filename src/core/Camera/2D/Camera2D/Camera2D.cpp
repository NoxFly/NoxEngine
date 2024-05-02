#include "Camera2D.hpp"

namespace NoxEngine {

    Camera2D::Camera2D():
        Camera(V3D(0, 0, 0), V3D(0, 0, 1), V3D(0, 1, 0))
    {}

    Camera2D::~Camera2D()
    {}

    V2D Camera2D::getPosition() const noexcept {
        return V2D(m_position.x, m_position.y);
    }

    float Camera2D::getZoom() const noexcept {
        return m_position.z;
    }

    void Camera2D::setPosition(const float x, const float y) noexcept {
        Camera::_setPosition(x, y, m_position.z);
    }

    void Camera2D::setPosition(const V2D& position) noexcept {
        Camera::_setPosition(position.x, position.y, m_position.z);
    }

    void Camera2D::zoomIn(const float zoom, const unsigned int duration) noexcept {
        Camera::_setPosition(m_position.x, m_position.y, m_position.z + zoom);
        (void)duration;
    }

    void Camera2D::zoomOut(const float zoom, const unsigned int duration) noexcept {
        Camera::_setPosition(m_position.x, m_position.y, m_position.z - zoom);
        (void)duration;
    }

    void Camera2D::setZoom(const float zoom, const unsigned int duration) noexcept {
        Camera::_setPosition(m_position.x, m_position.y, zoom);
        (void)duration;
    }

    void Camera2D::move(const V2D& offset, const unsigned int duration) {
        if(duration == 0) {
            setPosition(offset);
        }
    }

    void Camera2D::moveTo(const V2D& position, const unsigned int duration) {
        if(duration == 0) {
            setPosition(position);
        }
    }

}
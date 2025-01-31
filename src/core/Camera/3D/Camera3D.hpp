/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef CAMERA3D_HPP
#define CAMERA3D_HPP

#include <glm/glm.hpp>

#include "core/engine.typedef.hpp"
#include "core/Camera/Camera.hpp"
#include "core/MatricesMVP/Matrices.hpp"


namespace NoxEngine {

    class Camera3D: public Camera<V3D> {
        public:
            explicit Camera3D(const float left, const float right, const float top, const float bottom, const float near, const float far):
                Camera(),
                m_near(near), m_far(far)
            {
                m_matrix = Matrices<V3D>(left, right, top, bottom, m_near, m_far, m_position, m_verticalAxis);
            }

            explicit Camera3D(const float fov, const float aspect, const float near, const float far) :
                Camera(),
                m_near(near), m_far(far)
            {
                m_matrix = Matrices<V3D>(fov, aspect, m_near, m_far, m_position, m_verticalAxis);
            }
            
            V3D getPosition() const noexcept {
                return m_position;
            }

            void setPosition(const float x, const float y, const float z) noexcept {
                Camera::_setPosition(x, y, z);
            }

            void setPosition(const V3D& position) noexcept {
                Camera::_setPosition(position);
            };

            virtual void move(const V3D& offset, const unsigned int duration = 0) = 0;
            virtual void moveTo(const V3D& position, const unsigned int duration = 0) = 0;

            inline virtual void update() noexcept override {
                Camera::update();
            }

        protected:
            double m_near, m_far;
    };

}

#endif // CAMERA_3D_HPP
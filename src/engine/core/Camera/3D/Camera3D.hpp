#ifndef CAMERA3D_HPP
#define CAMERA3D_HPP

#include <glm/glm.hpp>

#include "core/engine.typedef.hpp"
#include "core/Camera/Camera.hpp"
#include "core/MatricesMVP/Matrices.hpp"


namespace NoxEngine {

    class Camera3D: public Camera<V3D> {
        public:
            Camera3D(float left, float right, float top, float bottom, float near, float far) :
                Camera(),
                m_near(near), m_far(far)
            {
                m_matrix = Matrices<V3D>(left, right, top, bottom, m_near, m_far, m_position, m_verticalAxis);
            }

            Camera3D(float fov, float aspect, float near, float far) :
                Camera(),
                m_near(near), m_far(far)
            {
                m_matrix = Matrices<V3D>(fov, aspect, m_near, m_far, m_position, m_verticalAxis);
            }
            
            virtual ~Camera3D() {};

            V3D getPosition() const {
                return m_position;
            }

            void setPosition(const float x, const float y, const float z) {
                Camera::_setPosition(x, y, z);
            }

            void setPosition(const V3D& position) {
                Camera::_setPosition(position);
            };

            virtual void move(const V3D& offset, unsigned int duration = 0) = 0;
            virtual void moveTo(const V3D& position, unsigned int duration = 0) = 0;

        protected:
            double m_near, m_far;
    };

}

#endif // CAMERA_3D_HPP
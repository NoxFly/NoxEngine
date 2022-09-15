#ifndef CAMERA3D_HPP
#define CAMERA3D_HPP

#include <glm/glm.hpp>

#include "engine.typedef.hpp"
#include "Camera.hpp"
#include "Object3D.hpp"
#include "Matrices3D.hpp"


namespace NoxEngine {

    class Camera3D: public Camera<V3D> {
        public:
            Camera3D(double fov, double aspect, double near, double far):
                Camera(),
                m_fov(fov), m_aspect(aspect), m_near(near), m_far(far)
            {
                m_matrix = Matrices3D(m_fov, m_aspect, m_near, m_far, m_position, m_verticalAxis);
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

            virtual void move(const V3D& offset, uint duration = 0) = 0;
            virtual void moveTo(const V3D& position, uint duration = 0) = 0;

        protected:
            double m_fov, m_aspect, m_near, m_far;
    };

}

#endif // CAMERA_3D_HPP
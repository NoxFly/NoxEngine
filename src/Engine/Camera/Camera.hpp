#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Object3D.hpp"
#include "MatricesMVP.hpp"

class Camera: public Object3D {
    public:
        Camera(double fov, double aspect, double near, double far):
            Object3D(),
            m_fov(fov), m_aspect(aspect), m_near(near), m_far(far), m_verticalAxis(0, 1, 0),
            m_mvp(m_fov, m_aspect, m_near, m_far, m_position, m_verticalAxis) {}
        virtual ~Camera() {};

        virtual void update() = 0;

        MatricesMVP* getMVP() {
            return &m_mvp;
        }

    protected:
        double m_fov, m_aspect, m_near, m_far;
        glm::vec3 m_verticalAxis;
        MatricesMVP m_mvp;
};

#endif // CAMERA_HPP
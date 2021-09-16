#ifndef CAMERA_H
#define CAMERA_H

#include "Object3D.h"
#include "MatricesMVP.h"

class Camera: public Object3D {
    public:
        Camera(double fov, double aspect, double near, double far):
            Object3D(0, 0, 0),
            m_fov(fov), m_aspect(aspect), m_near(near), m_far(far), m_verticalAxis(0, 1, 0),
            m_mvp(m_fov, m_aspect, m_near, m_far, m_position, m_verticalAxis) {}
        virtual ~Camera() {};

        MatricesMVP* getMVP() {
            return &m_mvp;
        }

    protected:
        double m_fov, m_aspect, m_near, m_far;
        glm::vec3 m_verticalAxis;
        MatricesMVP m_mvp;
};

#endif // CAMERA_H
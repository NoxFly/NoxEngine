#ifndef PERSPECTIVE_CAMERA_H
#define PERSPECTIVE_CAMERA_H

#include <glm/glm.hpp>

#include "Camera.h"


class PerspectiveCamera: public Camera {
    public:
        PerspectiveCamera(double fov, double aspect, double near, double far);
        ~PerspectiveCamera();

        void lookAt(const glm::vec3 eye, const glm::vec3 center, const glm::vec3 up);
        void lookAt(const float x, const float y, const float z);
        void lookAt(const glm::vec3& target);

        void orientate(const glm::vec2& dir);

        void setPosition(const float x, const float y, const float z);
        void setPosition(const glm::vec3& position);


    private:
        float m_phi, m_theta;
        glm::vec3 m_orientation, m_lateralDisplacement, m_target;
        float m_sensivity;

        void updatelookAt();
};

#endif // PERSPECTIVE_CAMERA_H
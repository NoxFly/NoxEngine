#ifndef PERSPECTIVE_CAMERA_HPP
#define PERSPECTIVE_CAMERA_HPP

#include <glm/glm.hpp>


#include "Camera.hpp"


class PerspectiveCamera: public Camera {
    public:
        PerspectiveCamera(double fov, double aspect, double near, double far);
        ~PerspectiveCamera();

        void lookAt(const glm::vec3 eye, const glm::vec3 target, const glm::vec3 up);
        void lookAt(const float x, const float y, const float z);
        void lookAt(const glm::vec3& target);

        void orientate(const glm::vec2& dir);

        void setPosition(const float x, const float y, const float z);
        void setPosition(const glm::vec3& position);

        void update();

    private:
        float m_phi, m_theta;
        glm::vec3 m_orientation, m_lateralDisplacement, m_target;
        float m_sensivity;
};

#endif // PERSPECTIVE_CAMERA_HPP
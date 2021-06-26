#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

#include "Input.h"

class Camera {
	public:
		Camera();
		~Camera();

        void orientate(const glm::vec2& dir);
        void update(const Input& input);
        void lookAt(glm::mat4& view);
        void lookAt(const glm::vec3 eye, const glm::vec3 center, const glm::vec3 up);
        void setTarget(const glm::vec3& target);
        void setPosition(const glm::vec3& position);
        void setSensivity(const float sensivity);
        void setSpeed(const float speed);

        void move(const glm::vec3 offset);
        void move(const float x, const float y, const float z);
        void move(const int x, const int y, const int z);
        
        void addUp(const glm::vec3 offset);
        void addUp(const float x, const float y, const float z);
        void addUp(const int x, const int y, const int z);
        
        void addTarget(const glm::vec3 offset);
        void addTarget(const float x, const float y, const float z);
        void addTarget(const int x, const int y, const int z);

        float getSensivity() const;
        float getSpeed() const;

    private:
        float m_phi, m_theta;
        glm::vec3 m_orientation;
        glm::vec3 m_verticalAxis, m_lateralDisplacement;
        glm::vec3 m_position, m_target;
        float m_sensivity, m_maxSpeed, m_speed, m_velocity;
};

#endif // CAMERA_H
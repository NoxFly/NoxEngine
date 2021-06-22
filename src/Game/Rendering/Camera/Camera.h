#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

#include "Input.h"

class Camera {
	public:
		Camera();
		Camera(glm::vec3 position, glm::vec3 target, glm::vec3 verticalAxis, float sensivity, float maxSpeed);
		~Camera();

        void orientate(const glm::vec2& mouseDir);
        void update(const Input& input);
        void lookAt(glm::mat4& modelView);
        void setTarget(const glm::vec3& target);
        void setPosition(const glm::vec3& position);
        void setSensivity(const float sensivity);
        void setSpeed(const float speed);
        // void setGravity(float gravity);

        float getSensivity() const;
        float getSpeed() const;

    private:
        float phi, theta;
        glm::vec3 orientation;
        glm::vec3 verticalAxis, lateralDisplacement;
        glm::vec3 position, target;
        float sensivity, maxSpeed, speed, velocity;
};

#endif // CAMERA_H
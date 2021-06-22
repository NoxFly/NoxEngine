#include "Camera.h"

#include <glm/gtx/transform.hpp>

#include "Console.h"

Camera::Camera():
    phi(0.0), theta(0.0), orientation(), verticalAxis(0, 0, 1),
    lateralDisplacement(), position(), target()
{

}

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 verticalAxis, float sensivity, float maxSpeed):
    phi(0.0), theta(0.0), orientation(), verticalAxis(verticalAxis),
    lateralDisplacement(), position(position), target(target),
    sensivity(sensivity), speed(0), maxSpeed(maxSpeed), velocity(1)
{
    
}

Camera::~Camera() {

}

void Camera::orientate(const glm::vec2& mouseDir) {
    phi += -mouseDir.y * sensivity;
    theta += -mouseDir.x * sensivity;

    if(phi > 89.0) phi = 89.0;
    else if(phi < -89.0) phi = -89.0;

    const float PI = 3.14;

    float phiRadian = phi * PI / 180;
    float thetaRadian = theta * PI / 180;

    // vertical axis = horizontal axis
    if(verticalAxis.x == 1.0) {
        orientation.x = sin(phiRadian);
        orientation.y = cos(phiRadian) * cos(thetaRadian);
        orientation.z = cos(phiRadian) * sin(thetaRadian);
    }
    // Y axis
    else if(verticalAxis.y == 1.0) {
        orientation.x = cos(phiRadian) * sin(thetaRadian);
        orientation.y = sin(phiRadian);
        orientation.z = cos(phiRadian) * cos(thetaRadian);
    }
    // Z axis
    else {
        orientation.x = cos(phiRadian) * cos(thetaRadian);
        orientation.y = cos(phiRadian) * sin(thetaRadian);
        orientation.z = sin(phiRadian);
    }

    lateralDisplacement = normalize(cross(verticalAxis, orientation));
}

void Camera::update(const Input& input) {
    if(input.isMouseMoving())
		orientate(input.getMouseDir());

    glm::vec3 movementVector;
    bool isMoving = false;

    if(isMoving |= (input.isKeyDown(SDL_SCANCODE_UP) || input.isKeyDown(SDL_SCANCODE_W)))
        movementVector = orientation;

    if(isMoving |= (input.isKeyDown(SDL_SCANCODE_DOWN) || input.isKeyDown(SDL_SCANCODE_S)))
        movementVector = orientation * -1.0f;

    if(isMoving |= (input.isKeyDown(SDL_SCANCODE_LEFT) || input.isKeyDown(SDL_SCANCODE_A)))
        movementVector = lateralDisplacement;

    if(isMoving |= (input.isKeyDown(SDL_SCANCODE_RIGHT) || input.isKeyDown(SDL_SCANCODE_D)))
        movementVector = lateralDisplacement * -1.0f;

    if(isMoving |= (input.isKeyDown(SDL_SCANCODE_SPACE)))
        movementVector = verticalAxis;

    if(isMoving |= (input.isKeyDown(SDL_SCANCODE_LCTRL)))
        movementVector = verticalAxis * -1.0f;

    if(isMoving) {
        speed += velocity;

        if(speed > maxSpeed) 
		    speed = maxSpeed;

        position += movementVector * speed;
    }

	else {
        speed -= velocity;
        
        if(speed < 0)
            speed = 0;
	}

	target = position + orientation;
}

void Camera::lookAt(glm::mat4& modelview) {
    modelview = glm::lookAt(position, target, verticalAxis);
}

void Camera::setTarget(const glm::vec3& target) {
    orientation = target - position;
    orientation = normalize(orientation);

    if(verticalAxis.x == 1.0) {
        phi = asin(orientation.x);
        theta = acos(orientation.y / cos(phi));

        if(orientation.y < 0)
            theta *= 1;
    }

    else if(verticalAxis.y == 1.0) {
        phi = asin(orientation.y);
        theta = acos(orientation.z / cos(phi));

        if(orientation.z < 0)
            theta *= -1;
    }

    else {
        phi = asin(orientation.x);
        theta = acos(orientation.z / cos(phi));

        if(orientation.z < 0)
            theta *= -1;
    }

    float PI = 3.14;

    phi *= 180 / PI;
    theta *= 180 / PI;
}

void Camera::setPosition(const glm::vec3& position) {
    this->position = position;
    target = position + orientation;
}

float Camera::getSensivity() const {
    return sensivity;
}

float Camera::getSpeed() const {
    return speed;
}

void Camera::setSensivity(const float sensivity) {
    this->sensivity = sensivity;
}

void Camera::setSpeed(const float speed) {
    this->speed = speed;
}
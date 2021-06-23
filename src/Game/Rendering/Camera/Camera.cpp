#include "Camera.h"

#include <glm/gtx/transform.hpp>

#include "Console.h"

Camera::Camera():
    m_phi(0.0), m_theta(0.0), m_orientation(), m_verticalAxis(0, 0, 1),
    m_lateralDisplacement(), m_position(), m_target(),
    m_sensivity(1), m_maxSpeed(1), m_speed(1), m_velocity(1)
{

}

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 verticalAxis, float sensivity, float maxSpeed):
    m_phi(0.0), m_theta(0.0), m_orientation(), m_verticalAxis(verticalAxis),
    m_lateralDisplacement(), m_position(position), m_target(target),
    m_sensivity(sensivity), m_maxSpeed(maxSpeed), m_speed(0), m_velocity(1)
{
    
}

Camera::~Camera() {

}

// orientates the camera's target point
void Camera::orientate(const glm::vec2& mouseDir) {
    m_phi += -mouseDir.y * m_sensivity;
    m_theta += -mouseDir.x * m_sensivity;

    if(m_phi > 89.0) m_phi = 89.0;
    else if(m_phi < -89.0) m_phi = -89.0;

    const float PI = 3.14;

    float phiRadian = m_phi * PI / 180;
    float thetaRadian = m_theta * PI / 180;

    // vertical axis = horizontal axis
    if(m_verticalAxis.x == 1.0) {
        m_orientation.x = sin(phiRadian);
        m_orientation.y = cos(phiRadian) * cos(thetaRadian);
        m_orientation.z = cos(phiRadian) * sin(thetaRadian);
    }
    // Y axis
    else if(m_verticalAxis.y == 1.0) {
        m_orientation.x = cos(phiRadian) * sin(thetaRadian);
        m_orientation.y = sin(phiRadian);
        m_orientation.z = cos(phiRadian) * cos(thetaRadian);
    }
    // Z axis
    else {
        m_orientation.x = cos(phiRadian) * cos(thetaRadian);
        m_orientation.y = cos(phiRadian) * sin(thetaRadian);
        m_orientation.z = sin(phiRadian);
    }

    m_lateralDisplacement = normalize(cross(m_verticalAxis, m_orientation));
    m_target = m_position + m_orientation;
}

// Updates the camera position and look
void Camera::update(const Input& input) {
    if(input.isMouseMoving())
		orientate(input.getMouseDir());

    glm::vec3 movementVector;
    bool isMoving = false;

    if(isMoving |= (input.isKeyDown(SDL_SCANCODE_UP) || input.isKeyDown(SDL_SCANCODE_W)))
        movementVector = m_orientation;

    if(isMoving |= (input.isKeyDown(SDL_SCANCODE_DOWN) || input.isKeyDown(SDL_SCANCODE_S)))
        movementVector = m_orientation * -1.0f;

    if(isMoving |= (input.isKeyDown(SDL_SCANCODE_LEFT) || input.isKeyDown(SDL_SCANCODE_A)))
        movementVector = m_lateralDisplacement;

    if(isMoving |= (input.isKeyDown(SDL_SCANCODE_RIGHT) || input.isKeyDown(SDL_SCANCODE_D)))
        movementVector = m_lateralDisplacement * -1.0f;

    if(isMoving |= (input.isKeyDown(SDL_SCANCODE_SPACE)))
        movementVector = m_verticalAxis;

    if(isMoving |= (input.isKeyDown(SDL_SCANCODE_LCTRL)))
        movementVector = m_verticalAxis * -1.0f;

    if(isMoving) {
        m_speed += m_velocity;

        if(m_speed > m_maxSpeed) 
		    m_speed = m_maxSpeed;

        m_position += movementVector * m_speed;
    }

	else {
        m_speed -= m_velocity;
        
        if(m_speed < 0)
            m_speed = 0;
	}

	m_target = m_position + m_orientation;
}

// Defines the camera's target
void Camera::setTarget(const glm::vec3& target) {
    m_orientation = target - m_position;
    m_orientation = normalize(m_orientation);

    if(m_verticalAxis.x == 1.0) {
        m_phi = asin(m_orientation.x);
        m_theta = acos(m_orientation.y / cos(m_phi));

        if(m_orientation.y < 0)
            m_theta *= 1;
    }

    else if(m_verticalAxis.y == 1.0) {
        m_phi = asin(m_orientation.y);
        m_theta = acos(m_orientation.z / cos(m_phi));

        if(m_orientation.z < 0)
            m_theta *= -1;
    }

    else {
        m_phi = asin(m_orientation.x);
        m_theta = acos(m_orientation.z / cos(m_phi));

        if(m_orientation.z < 0)
            m_theta *= -1;
    }

    float PI = 3.14;

    m_phi *= 180 / PI;
    m_theta *= 180 / PI;
}


// Defines the camera's position
void Camera::setPosition(const glm::vec3& position) {
    this->m_position = position;
    m_target = position + m_orientation;
}


// Updates the view matrix. Normally called every update loop
void Camera::lookAt(glm::mat4& view) {
    view = glm::lookAt(m_position, m_target, m_verticalAxis);
}


// Set's the camera's look at. The view matrix will be upated in the loop
void Camera::lookAt(const glm::vec3 eye, const glm::vec3 center, const glm::vec3 up) {
    m_position = eye;
    m_target = center;
    m_verticalAxis = up;
}


// moves the camera's position with given offset
void Camera::move(const glm::vec3 offset) {
    m_position += offset;
}


// moves the camera's vertical axis with given offset
void Camera::addUp(const glm::vec3 offset) {
    m_verticalAxis += offset;
}


// moves the camera's target with given offset
void Camera::addTarget(const glm::vec3 offset) {
    m_target += offset;
}


// moves the camera's position with given offset
void Camera::move(const float x, const float y, const float z) {
    m_position += glm::vec3(x, y, z);
}


// moves the camera's position with given offset
void Camera::move(const int x, const int y, const int z) {
    m_position += glm::vec3(x, y, z);
}


// moves the camera's vertical axis with given offset
void Camera::addUp(const float x, const float y, const float z) {
    m_verticalAxis += glm::vec3(x, y, z);
}

// moves the camera's vertical axis with given offset
void Camera::addUp(const int x, const int y, const int z) {
    m_verticalAxis += glm::vec3(x, y, z);
}


// moves the camera's target with given offset
void Camera::addTarget(const float x, const float y, const float z) {
    m_target += glm::vec3(x, y, z);
}


// moves the camera's target with given offset
void Camera::addTarget(const int x, const int y, const int z) {
    m_target += glm::vec3(x, y, z);
}


float Camera::getSensivity() const {
    return m_sensivity;
}


float Camera::getSpeed() const {
    return m_speed;
}


void Camera::setSensivity(const float sensivity) {
    this->m_sensivity = sensivity;
}


void Camera::setSpeed(const float speed) {
    this->m_speed = speed;
}
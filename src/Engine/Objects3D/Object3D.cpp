#include "Object3D.hpp"

Object3D::Object3D():
    Object3D(0, 0, 0)
{

}

Object3D::Object3D(float x, float y, float z):
    Object3D(glm::vec3(x, y, z))
{

}

Object3D::Object3D(glm::vec3& position):
    m_position(position), m_rotation(0, 0, 0),
    m_hasToTranslate(false), m_hasToRotate(false)
{

}

Object3D::Object3D(glm::vec3 position):
    m_position(position), m_rotation(0, 0, 0),
    m_hasToTranslate(false), m_hasToRotate(false)
{

}

Object3D::~Object3D() {

}

const glm::vec3& Object3D::getPosition() const {
    return m_position;
}

void Object3D::setPosition(const float x, const float y, const float z) {
    setPosition(glm::vec3(x, y, z));
}

void Object3D::setPosition(const glm::vec3& position) {
    m_position = position;
    m_hasToTranslate = m_position.x != 0 || m_position.y != 0 || m_position.y != 0;
}

const glm::vec3& Object3D::getRotation() const {
    return m_rotation;
}

void Object3D::setRotation(const float x, const float y, const float z) {
    setRotation(glm::vec3(x, y, z));
}

void Object3D::setRotation(const glm::vec3& rotation) {
    m_rotation = rotation;
    m_hasToRotate = m_rotation.x != 0 || m_rotation.y != 0 || m_rotation.z != 0;
}
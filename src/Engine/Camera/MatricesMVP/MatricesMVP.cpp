#include "MatricesMVP.h"


MatricesMVP::MatricesMVP(double fov, double aspect, double near, double far, const glm::vec3& position, const glm::vec3& verticalAxis):
    m_projection(glm::perspective(fov, aspect, near, far)),
    m_model(1.0f),
    m_view(glm::lookAt(position, glm::vec3(0, 0, 0), verticalAxis)),
    m_saves{},
    operationDone(true)
{

}

MatricesMVP::~MatricesMVP() {

}

void MatricesMVP::updateMVP() {
    m_MVP = m_projection * m_view * m_model;
    operationDone = false;
}

glm::mat4& MatricesMVP::getProjection() {
    return m_projection;
}

glm::mat4& MatricesMVP::getModel() {
    return m_model;
}

glm::mat4& MatricesMVP::getView() {
    return m_view;
}

glm::mat4& MatricesMVP::getMVP() {
    if(operationDone)
        updateMVP();
    return m_MVP;
}

const glm::mat4& MatricesMVP::getMVP() const {
    return m_MVP;
}

void MatricesMVP::push() {
    m_saves.insert(m_saves.begin(), m_model);
}

void MatricesMVP::pop() {
    if(m_saves.size() > 0) {
        m_model = m_saves.erase(m_saves.begin())[0];
        operationDone = true;
    }
}

void MatricesMVP::translate(int x, int y, int z) {
    translate(glm::vec3(x, y, z));
}

void MatricesMVP::translate(float x, float y, float z) {
    translate(glm::vec3(x, y, z));
}

void MatricesMVP::translate(const glm::vec3& translation) {
    m_model = glm::translate(m_model, translation);
    operationDone = true;
}


void MatricesMVP::rotate(int x, int y, int z) {
    rotate(glm::vec3(x, y, z));
}

void MatricesMVP::rotate(float x, float y, float z) {
    rotate(glm::vec3(x, y, z));
}

void MatricesMVP::rotate(const glm::vec3& rotation) {
    if(rotation.x != 0)
        m_model = glm::rotate(m_model, glm::radians(rotation.x), glm::vec3(0, 1, 0));

    if(rotation.y != 0)
        m_model = glm::rotate(m_model, glm::radians(rotation.y), glm::vec3(0, 0, 1));

    if(rotation.z != 0)
        m_model = glm::rotate(m_model, glm::radians(rotation.z), glm::vec3(1, 0, 0));
        
    operationDone = true;
}

void MatricesMVP::setView(const glm::mat4& lookAt) {
    m_view = lookAt;
}
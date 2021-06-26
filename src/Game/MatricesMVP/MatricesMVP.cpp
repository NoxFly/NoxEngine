#include "MatricesMVP.h"

MatricesMVP::MatricesMVP(double fovy, double aspect, double near, double far):
    m_projection(glm::perspective(fovy, aspect, near, far)),
    m_model(1.0f),
    m_view(glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0))),
    m_saves{}
{

}

MatricesMVP::~MatricesMVP() {

}

void MatricesMVP::updateMVP() {
    m_MVP = m_projection * m_view * m_model;
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
    return m_MVP;
}

const glm::mat4& MatricesMVP::getMVP() const {
    return m_MVP;
}

void MatricesMVP::push() {
    m_saves.insert(m_saves.begin(), m_view);
}

void MatricesMVP::pop() {
    if(m_saves.size() > 0)
        m_view = m_saves.erase(m_saves.begin())[0];
}

void MatricesMVP::translate(int x, int y, int z) {
    translate(glm::vec3(x, y, z));
}

void MatricesMVP::translate(float x, float y, float z) {
    translate(glm::vec3(x, y, z));
}

void MatricesMVP::translate(glm::vec3 translation) {
    m_view = glm::translate(m_view, translation);
}

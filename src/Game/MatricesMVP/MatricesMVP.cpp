#include "MatricesMVP.h"

MatricesMVP::MatricesMVP(double fovy, double aspect, double near, double far):
    m_projection(glm::perspective(fovy, aspect, near, far)),
    m_model(1.0f),
    m_view(glm::lookAt(glm::vec3(3, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)))
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
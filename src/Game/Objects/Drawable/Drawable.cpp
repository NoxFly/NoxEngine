#include "Drawable.h"

#include <iostream>

#include "Console.h"

ResourceHolder<Shader, std::string>* Drawable::shadersBank = 0;

void Drawable::setShadersBank(ResourceHolder<Shader, std::string>& shadersBank) {
    Drawable::shadersBank = &shadersBank;
}

Drawable::Drawable():
    shader(0),
    wireframe(false),
    vertexNumber(0),
    VBO(0), VAO(0)
{

}

Drawable::Drawable(float* vertices, GLuint verticesSize):
    Drawable(vertices, 0, verticesSize)
{
    
}

Drawable::Drawable(float* vertices, float* colors, GLuint verticesSize):
    shader(0),
    wireframe(false),
    vertexNumber(0),
    VBO(0), VAO(0)
{
    load(vertices, colors, verticesSize);
}

Drawable::~Drawable() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Drawable::load(float* vertices, float* colors, GLuint verticesSize) {
    if(verticesSize % 3 != 0)
        return;

    size_t sizeVertices = sizeof(float) * 108;
    size_t sizeColors = (colors != 0)? sizeVertices : 0;

    vertexNumber = verticesSize / 3;
    
    if(glIsBuffer(VBO) == GL_TRUE)
        glDeleteBuffers(1, &VBO);

    if(glIsVertexArray(VAO) == GL_TRUE)
        glDeleteVertexArrays(1, &VAO);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

            glBufferData(GL_ARRAY_BUFFER, sizeVertices + sizeColors, 0, GL_STATIC_DRAW);

            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVertices, vertices);

            if(colors != 0)
                glBufferSubData(GL_ARRAY_BUFFER, sizeVertices, sizeColors, colors);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
            glEnableVertexAttribArray(0);

            if(colors != 0) {
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeVertices));
                glEnableVertexAttribArray(1);
            }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Drawable::setShader(const std::string& shaderName) {
    if(Drawable::shadersBank != 0 && Drawable::shadersBank->has(shaderName))
        this->shader = &Drawable::shadersBank->get(shaderName);
}

void Drawable::setWireframe(const bool wireframeState) {
    wireframe = wireframeState;
}

void Drawable::draw(glm::mat4& projection, glm::mat4& modelview) {
    if(!shader)
        return;
    
    glPolygonMode(GL_FRONT_AND_BACK, wireframe? GL_LINE : GL_FILL);

    glUseProgram(shader->getId());

        glBindVertexArray(VAO);

            glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "modelview"), 1, GL_FALSE, glm::value_ptr(modelview));
            glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

            glDrawArrays(GL_TRIANGLES, 0, vertexNumber);

        glBindVertexArray(0);

    glUseProgram(0);
}
#include "Drawable.h"

#include <iostream>

#include "Console.h"

ResourceHolder<Shader, std::string>* Drawable::shadersBank = 0;

// defines the pointer of the shader's resource holder
void Drawable::setShadersBank(ResourceHolder<Shader, std::string>& shadersBank) {
    Drawable::shadersBank = &shadersBank;
}

// empty entity
Drawable::Drawable():
    m_wireframe(false),
    m_shader(0),
    m_VBO(0), m_VAO(0),
    m_vertexNumber(0)
{

}

// creates entity only with vertices
Drawable::Drawable(float* vertices, GLuint verticesSize):
    Drawable(vertices, 0, verticesSize)
{
    
}

// creates entity with vertices and colors
Drawable::Drawable(float* vertices, float* colors, GLuint verticesSize):
    m_wireframe(false),
    m_shader(0),
    m_VBO(0), m_VAO(0),
    m_vertexNumber(0)
{
    load(vertices, colors, verticesSize);
}

Drawable::~Drawable() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}


// loads the vertices, colors and textures of the entity in the memory
void Drawable::load(float* vertices, float* colors, GLuint verticesSize) {
    if(verticesSize % 3 != 0)
        return;

    size_t sizeVertices = sizeof(float) * verticesSize;
    size_t sizeColors = (colors != 0)? sizeVertices : 0;

    m_vertexNumber = verticesSize / 3;
    
    // delete possibly existing older VBO & VAO
    if(glIsBuffer(m_VBO) == GL_TRUE)
        glDeleteBuffers(1, &m_VBO);

    if(glIsVertexArray(m_VAO) == GL_TRUE)
        glDeleteVertexArrays(1, &m_VAO);
    //

    // generates new VBO & VAO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    // bind the VAO and the VBO
    glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            
            // allocate memory
            glBufferData(GL_ARRAY_BUFFER, sizeVertices + sizeColors, 0, GL_STATIC_DRAW);

            // transfert data for vertices
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVertices, vertices);

            // transfert data for colors
            if(sizeColors > 0)
                glBufferSubData(GL_ARRAY_BUFFER, sizeVertices, sizeColors, colors);

            // access to coords in the memory and lock these
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
            glEnableVertexAttribArray(0);

            if(sizeColors > 0) {
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeVertices));
                glEnableVertexAttribArray(1);
            }

        // unlock VBO & VAO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


// defines the shader of the entity
void Drawable::setShader(const std::string& shaderName) {
    if(Drawable::shadersBank != 0 && Drawable::shadersBank->has(shaderName)) {
        this->m_shader = &Drawable::shadersBank->get(shaderName);
    }
}


// enables or disables the wireframe mode
void Drawable::setWireframe(const bool wireframeState) {
    m_wireframe = wireframeState;
}


// draw the entity on given MVP
void Drawable::draw(glm::mat4& MVP) {
    // can't draw if the entity has not a shader
    if(!m_shader)
        return;
    
    // wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, m_wireframe? GL_LINE : GL_FILL);

    // lock shader
    glUseProgram(m_shader->getId());
        // lock VAO
        glBindVertexArray(m_VAO);
            // sends the matrices
            glUniformMatrix4fv(glGetUniformLocation(m_shader->getId(), "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
            // renders
            glDrawArrays(GL_TRIANGLES, 0, m_vertexNumber);
        // unlock VAO
        glBindVertexArray(0);
    // unlock shader
    glUseProgram(0);
}
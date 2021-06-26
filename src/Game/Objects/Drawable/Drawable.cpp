#include "Drawable.h"

#include <iostream>

#include "Console.h"

ResourceHolder<Shader, std::string>* Drawable::shadersBank = 0;
ResourceHolder<Texture, std::string>* Drawable::texturesBank = 0;

// defines the pointer of the shader's resource holder
void Drawable::setShadersBank(ResourceHolder<Shader, std::string>& shadersBank) {
    Drawable::shadersBank = &shadersBank;
}

// defines the pointer of the textures's resource holder
void Drawable::setTexturesBank(ResourceHolder<Texture, std::string>& texturesBank) {
    Drawable::texturesBank = &texturesBank;
}


// empty entity
Drawable::Drawable():
    m_wireframe(false),
    m_geometry(),
    m_shader(0),
    m_VBO(0), m_VAO(0),
    m_vertexNumber(0)
{

}

Drawable::Drawable(Geometry& geometry):
    m_wireframe(false),
    m_geometry(geometry),
    m_shader(0),
    m_VBO(0), m_VAO(0),
    m_vertexNumber(0)
{
    load();
}

Drawable::~Drawable() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

Drawable& Drawable::operator=(Drawable const &copy) {
    m_geometry = copy.getGeometry();
    m_shader = copy.getShader();
    m_texture = copy.getTexture();

    load();

    return *this;
}

Drawable::Drawable(Drawable const &copy) {
    m_geometry = copy.getGeometry();
    m_shader = copy.getShader();
    m_texture = copy.getTexture();

    load();
}

// loads the vertices, colors and textures of the entity in the memory
void Drawable::load() {
    if(m_geometry.verticesSize % 3 != 0 || m_geometry.vertices == 0)
        return;

    size_t sizeVertices = sizeof(float) * m_geometry.verticesSize;
    size_t sizeColors = (m_geometry.colors != 0)? sizeVertices : 0;
    size_t sizeTex = (m_geometry.textures != 0)? m_geometry.texturesSize : 0;

    m_vertexNumber = m_geometry.verticesSize / 3;
    
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
            glBufferData(GL_ARRAY_BUFFER, sizeVertices + sizeColors + sizeTex, 0, GL_STATIC_DRAW);

            // transfert data for vertices
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVertices, m_geometry.vertices);

            // transfert data for colors
            if(sizeColors > 0)
                glBufferSubData(GL_ARRAY_BUFFER, sizeVertices, sizeColors, m_geometry.colors);

            // transfert data for textures
            if(sizeTex > 0)
                glBufferSubData(GL_ARRAY_BUFFER, sizeVertices + sizeColors, sizeTex, m_geometry.textures);

            // access to coords in the memory and lock these
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
            glEnableVertexAttribArray(0);

            if(sizeColors > 0) {
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeVertices));
                glEnableVertexAttribArray(1);
            }

            if(sizeTex > 0) {
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeVertices + sizeColors));
                glEnableVertexAttribArray(2);
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


void Drawable::setTexture(const std::string& texName) {
    if(Drawable::texturesBank != 0 && Drawable::texturesBank->has(texName)) {
        this->m_texture = &Drawable::texturesBank->get(texName);
    }
}


// enables or disables the wireframe mode
void Drawable::setWireframe(const bool wireframeState) {
    m_wireframe = wireframeState;
}

bool Drawable::isWireframed() const {
    return m_wireframe;
}

Geometry Drawable::getGeometry() const {
    return m_geometry;
}

Shader* Drawable::getShader() const {
    return m_shader;
}

Texture* Drawable::getTexture() const {
    return m_texture;
}


// draw the entity on given MVP
void Drawable::draw(glm::mat4& MVP) {
    // can't draw if the entity has not a shader
    if(!m_shader)
        return;
    
    // wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, isWireframed()? GL_LINE : GL_FILL);

    // lock shader
    m_shader->use();
        // lock VAO
        glBindVertexArray(m_VAO);

            // sends the matrices
            glUniformMatrix4fv(glGetUniformLocation(m_shader->getId(), "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));

            if(m_geometry.texturesSize > 0 && m_texture != 0)
                glBindTexture(GL_TEXTURE_2D, m_texture->getID());

            // renders
            glDrawArrays(GL_TRIANGLES, 0, m_vertexNumber);

            if(m_geometry.texturesSize > 0 && m_texture != 0)
                glBindTexture(GL_TEXTURE_2D, 0);

        // unlock VAO
        glBindVertexArray(0);
    // unlock shader
    glUseProgram(0);
}
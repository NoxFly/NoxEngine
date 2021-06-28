#include "Drawable.h"

#include <iostream>

#include "Console.h"


// empty entity
Drawable::Drawable():
    m_wireframe(false),
    m_geometry(),
    m_material(),
    m_VBO(0), m_VAO(0),
    m_vertexNumber(0)
{

}

Drawable::Drawable(Geometry& geometry, Material& material):
    Drawable(geometry, material, true)
{

}

Drawable::Drawable(Geometry& geometry, Material& material, bool hasToLoad):
    m_wireframe(false),
    m_geometry(geometry),
    m_material(material),
    m_VBO(0), m_VAO(0),
    m_vertexNumber(0)
{
    if(hasToLoad)
        load();
}

Drawable::~Drawable() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

Drawable& Drawable::operator=(Drawable const &copy) {
    m_geometry = copy.getGeometry();
    m_material = copy.getMaterial();

    return *this;
}

Drawable::Drawable(Drawable const &copy) {
    m_geometry = copy.getGeometry();
    m_material = copy.getMaterial();
    
    load();
}


// defines the shader of the entity
void Drawable::setShader(const std::string& shaderName) {
    m_material.setShader(shaderName);
}


void Drawable::setTexture(const std::string& texName) {
    m_material.setTextures(texName);
}

Shader* Drawable::getShader() const {
    return m_material.getShader();
}

Texture* Drawable::getTexture() const {
    if(m_material.getTextures().size() > 0)
        return m_material.getTextures()[0];
    
    return nullptr;
}

Geometry& Drawable::getGeometry() {
    return m_geometry;
}

Material& Drawable::getMaterial() {
    return m_material;
}

Geometry Drawable::getGeometry() const {
    return m_geometry;
}

Material Drawable::getMaterial() const {
    return m_material;
}

// enables or disables the wireframe mode
void Drawable::setWireframe(const bool wireframeState) {
    m_wireframe = wireframeState;
}

bool Drawable::isWireframed() const {
    return m_wireframe;
}


// loads the vertices, colors and textures of the entity in the memory
void Drawable::load() {
    // it needs some vertices and each vertex is 3D (x,y,z)
    if(m_geometry.vertices.array.size() == 0 || m_geometry.vertices.size % 3 != 0)
        return;

    size_t fsize = sizeof(float);

    size_t sizeVertices =   fsize * m_geometry.vertices.size;
    size_t sizeColors =     (m_geometry.colors.array.size() > 0)? fsize * m_geometry.colors.size : 0;
    size_t sizeTex =        (m_geometry.textures.array.size() > 0 && m_material.hasTexture())? fsize * m_geometry.textures.size : 0;

    m_vertexNumber = sizeVertices / fsize / 3;
    
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
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVertices, m_geometry.vertices.array.data());

            // transfert data for colors
            if(sizeColors > 0)
                glBufferSubData(GL_ARRAY_BUFFER, sizeVertices, sizeColors, m_geometry.colors.array.data());

            // transfert data for textures
            if(sizeTex > 0)
                glBufferSubData(GL_ARRAY_BUFFER, sizeVertices + sizeColors, sizeTex, m_geometry.textures.array.data());

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


// draw the entity on given MVP
void Drawable::draw(glm::mat4& MVP) {
    Shader* shader = m_material.getShader();

    // can't draw if the entity has not a shader
    if(!shader)
        return;

    bool hasTexture = m_material.hasTexture();
    
    // wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, isWireframed()? GL_LINE : GL_FILL);

    // lock shader
    shader->use();
        // lock VAO
        glBindVertexArray(m_VAO);

            // sends the matrices
            glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));

            if(hasTexture)
                glBindTexture(GL_TEXTURE_2D, m_material.getTextures()[0]->getID());

            // renders
            glDrawArrays(GL_TRIANGLES, 0, m_vertexNumber);

            if(hasTexture)
                glBindTexture(GL_TEXTURE_2D, 0);

        // unlock VAO
        glBindVertexArray(0);
    // unlock shader
    glUseProgram(0);
}
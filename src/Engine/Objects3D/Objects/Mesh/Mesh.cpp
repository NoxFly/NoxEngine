#include "Mesh.h"


Mesh::Mesh(Geometry& geometry, Material& material):
   Object(geometry, material)
{
    if(!load())
        std::cout << "Failed to load Mesh" << std::endl;
}

Mesh::~Mesh() {

}

bool Mesh::load() {
    if(m_material.getShader() == nullptr)
        return false;

    const int nV = m_geometry.getVertices().size();

    if(nV == 0 || nV % 3 != 0)
        return false;

    const int nC = m_geometry.getColors().size();
    const int nT = m_geometry.getTextures().size();
    
    if((nC > 0 && nC % 3 != 0) || (nT > 0 && nT % 3 != 0))
        return false;


    // nC == 0 : no colors
    // nT : no textures

    size_t fsize = sizeof(float);

    size_t vSize = fsize * nV;
    size_t cSize = fsize * nC;
    size_t tSize = fsize * nT;

    m_vertexNumber = nV / 3;

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
            glBufferData(GL_ARRAY_BUFFER, vSize + cSize + tSize, 0, GL_STATIC_DRAW);

            // transfert data for vertices
            glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, m_geometry.getVertices().data());

            // transfert data for colors
            if(cSize > 0)
                glBufferSubData(GL_ARRAY_BUFFER, vSize, cSize, m_geometry.getColors().data());

            // transfert data for textures
            if(tSize > 0)
                glBufferSubData(GL_ARRAY_BUFFER, vSize + cSize, tSize, m_geometry.getTextures().data());

            // access to coords in the memory and lock these
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
            glEnableVertexAttribArray(0);

            if(cSize > 0) {
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vSize));
                glEnableVertexAttribArray(1);
            }

            if(tSize > 0) {
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vSize + cSize));
                glEnableVertexAttribArray(2);
            }

        // unlock VBO & VAO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    return true;
}

void Mesh::draw(MatricesMVP* mvp) {
    if(m_material.getShader() == 0)
        return;

    bool hasTexture = m_material.getTextures().size() > 0;
    
    // wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, m_material.isWireframed()? GL_LINE : GL_FILL);

    (void)mvp;
    (void)hasTexture;

    // lock shader
    m_material.getShader()->use();
        // lock VAO
        glBindVertexArray(m_VAO);

            // sends the matrices
            glUniformMatrix4fv(glGetUniformLocation(m_material.getShader()->getId(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp->getMVP()));

            // if(hasTexture)
            //     glBindTexture(GL_TEXTURE_2D, m_material.getTextures()[0]->getID());

            // renders
            glDrawArrays(GL_TRIANGLES, 0, m_vertexNumber);

            // if(hasTexture)
            //     glBindTexture(GL_TEXTURE_2D, 0);

        // unlock VAO
        glBindVertexArray(0);
    // unlock shader
    glUseProgram(0);
}
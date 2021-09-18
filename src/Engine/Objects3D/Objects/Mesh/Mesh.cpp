#include "Mesh.h"

#include <iostream>


Mesh::Mesh(const Geometry& geometry, const Material& material):
   Object(geometry, material)
{
    if(!load())
        std::cout << "Failed to load Mesh" << std::endl;
}

Mesh::~Mesh() {

}

bool Mesh::load() {
    const size_t nV = m_geometry.getVertices().size();

    if(nV == 0 || nV % 3 != 0)
        return false;

    const size_t nC = m_material.getColors().size();
    const size_t nT = m_geometry.getTextures().size();

    
    if(nT > 0 && nT % 3 != 0)
        return false;


    // nC == 0 : no colors
    // nT : no textures

    const size_t fsize = sizeof(float);

    const size_t vSize = fsize * nV;
    const size_t cSize = fsize * nC * 3; // cause size is number of Color, which's composed by r,g,b[,a]
    const size_t tSize = fsize * nT;

    m_vertexNumber = nV / 3;


    std::vector<Color> ccolors = m_material.getColors();
    std::vector<float> colors = {};

    if(nC > 0) {
        const int d = m_vertexNumber / nC;

        //std::cout << ccolors.at(0) << std::endl;

        // for each color
        size_t k = 0;
        for(size_t i=0; i < nC; i++) {
            // color a % of the geometry
            int j = 0;
            while(++k <= m_vertexNumber && ++j <= d) {
                Color c = ccolors.at(i);
                colors.push_back((float)c.r / 255.0);
                colors.push_back((float)c.g / 255.0);
                colors.push_back((float)c.b / 255.0);
            }
        }

        /* const size_t n(colors.size());
        printf("%lld\n", n);

        for(size_t i=0; i < n; i += 3) {
            printf("%f, %f, %f", colors.at(i), colors.at(i+1), colors.at(i+2));

            if(i < n-1)
                printf(",");

            if(i % 9 == 0)
                printf("\n");
            else
                printf("   ");
        }
        std::cout << std::endl; */
    }


    // delete possibly existing older VBO & VAO
    if(glIsBuffer(m_VBO) == GL_TRUE)
        glDeleteBuffers(1, &m_VBO);

    if(glIsVertexArray(m_VAO) == GL_TRUE)
        glDeleteVertexArrays(1, &m_VAO);
    //

    // generates new VBO & VAO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    if(m_VBO == 0 || m_VAO == 0)
        return false;

    // bind the VAO and the VBO
    glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            
            // allocate memory
            glBufferData(GL_ARRAY_BUFFER, vSize + cSize + tSize, 0, GL_STATIC_DRAW);

            // transfert data for vertices
            glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, m_geometry.getVertices().data());

            // transfert data for colors
            if(nC > 0)
                glBufferSubData(GL_ARRAY_BUFFER, vSize, cSize, colors.data());

            // transfert data for textures
            if(nT > 0)
                glBufferSubData(GL_ARRAY_BUFFER, vSize + cSize, tSize, m_geometry.getTextures().data());

            // access to coords in the memory and lock these
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
            glEnableVertexAttribArray(0);

            if(nC > 0) {
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vSize));
                glEnableVertexAttribArray(1);
            }

            if(nT > 0) {
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
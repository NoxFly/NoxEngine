#include "GL/glew.h"

#include "utils.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace NoxEngine {

    template <Dimension D>
    Actor<D>::Actor():
        Actor(Geometry(), Material())
    {}

    template <Dimension D>
    Actor<D>::Actor(const Geometry& geometry, const Material& material):
        m_uuid(generateUUID()),
        m_dimension(Is2D<D>? 2: 3), m_is3D(m_dimension == 3),
        m_geometry(geometry), m_material(material),
        m_VBO(0), m_VAO(0),
        m_cullFace(GL_FRONT), m_polygonMode(GL_FILL),
        m_vertexNumber(0),
        m_rotation{},
        m_hasToTranslate(false), m_hasToRotate(false),
        m_isLoaded(false)
    {}

    // template <Dimension D>
    // Actor<D>::Actor(const Actor<D>& copy):
    //     Actor<D>({}, {})
    // {}

    // template <Dimension D>
    // const Actor<D>& Actor<D>::operator=(const Actor<D>& copy)
    // {

    // }

    template <Dimension D>
    Actor<D>::~Actor() {
        deleteVBO();
        deleteVAO();
    }

    template <Dimension D>
    void Actor<D>::deleteVBO() noexcept {
        if(glIsBuffer(m_VBO) == GL_TRUE)
            glDeleteBuffers(1, &m_VBO);
    }

    template <Dimension D>
    void Actor<D>::deleteVAO() noexcept {
        if(glIsVertexArray(m_VAO) == GL_TRUE)
            glDeleteVertexArrays(1, &m_VAO);
    }

    template <Dimension D>
    const std::string& Actor<D>::getUUID() const {
        return m_uuid;
    }

    template <Dimension D>
    Geometry* Actor<D>::getGeometry() {
        return &m_geometry;
    }

    template <Dimension D>
    Material* Actor<D>::getMaterial() {
        return &m_material;
    }

    template <Dimension D>
    const D& Actor<D>::getRotation() const {
        return m_rotation;
    }

    template <Dimension D>
    void Actor<D>::setCullFace(const GLenum cullFace) noexcept {
        m_cullFace = cullFace;
    }

    template <Dimension D>
    void Actor<D>::setPolygonMode(const GLenum polygonMode) noexcept {
        m_polygonMode = polygonMode;
    }

    template <Dimension D>
    void Actor<D>::setPosition(const D& position) noexcept {
        this->m_position = position;
        m_hasToTranslate = position.x != 0 || position.y != 0 || (m_is3D && position.z != 0);
    }

    template <Dimension D>
    void Actor<D>::setRotation(const D& rotation) noexcept {
        m_rotation = rotation;
        m_hasToRotate = rotation.x != 0 || rotation.y != 0 || (m_is3D && rotation.z != 0);
    }
    
    template <Dimension D>
    void Actor<D>::setPosition(const float x, const float y) noexcept requires Is2D<D> {
        setPosition(V2D(x, y));
    }

    template <Dimension D>
    void Actor<D>::setPosition(const float x, const float y, const float z) noexcept requires Is3D<D> {
        setPosition(V3D(x, y, z));
    }

    template <Dimension D>
    void Actor<D>::setRotation(const float rx, const float ry) noexcept requires Is2D<D> {
        setRotation(V2D(rx, ry));
    }

    template <Dimension D>
    void Actor<D>::setRotation(const float rx, const float ry, const float rz) noexcept requires Is3D<D> {
        setRotation(V3D(rx, ry, rz));
    }

    template <Dimension D>
    bool Actor<D>::load() {
        if(m_isLoaded)
            return true;

        const size_t nV = m_geometry.getVertices().size();

        if(nV == 0 || nV % m_dimension != 0)
            return false;

        // number of color (Color)
        const size_t nC = m_material.getColors().size();
        // number of textures (Texture*)
        const size_t nTex = m_material.getTextures().size();
        // material texture's number
        const size_t mnT = m_material.getTextures().size();
        // number of texture's points (x,y) on the geometry
        // if the material does not have any texture, then don't prepare the memory for nothing
        const size_t nT = mnT > 0 ? m_geometry.getTextures().size() : 0;

        // 6 coords = 1 2D triangle
        if(nTex > 0 && nT % 6 != 0)
            return false;


        m_vertexNumber = nV / m_dimension;


        std::vector<float> vertices = m_geometry.getVertices();
        std::vector<Color> ccolors = m_material.getColors();
        std::vector<float> textures = m_geometry.getTextures();
        std::vector<float> colors = {};

        if(nC > 0) {
            const int d = m_vertexNumber / nC;

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
                    colors.push_back((float)c.a / 255.0);
                }
            }
        }

        const size_t fsize = sizeof(float);

        const size_t vSize = fsize * nV;
        const size_t cSize = fsize * colors.size(); // cause size is number of Color, which's composed by r,g,b[,a]
        const size_t tSize = fsize * nT;


        // delete possibly existing older VBO & VAO
        deleteVBO();
        deleteVAO();
        //

        // generates new VBO & VAO
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        if(m_VBO == 0 || m_VAO == 0) {
            deleteVBO();
            deleteVAO();
            return false;
        }

        // glVertexAttribPointer()
        // index, size, type, normalize, stride (from/start), pointer (to/end)

        // bind the VAO and the VBO
        glBindVertexArray(m_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
                
                // allocate memory
                glBufferData(GL_ARRAY_BUFFER, vSize + cSize + tSize, 0, GL_STATIC_DRAW);

                // transfert data for vertices
                glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, vertices.data());

                // transfert data for colors
                if(nC > 0)
                    glBufferSubData(GL_ARRAY_BUFFER, vSize, cSize, colors.data());

                // transfert data for textures
                if(nT > 0)
                    glBufferSubData(GL_ARRAY_BUFFER, vSize + cSize, tSize, m_geometry.getTextures().data());

                // access to coords in the memory and lock these
                glVertexAttribPointer(0, m_dimension, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
                glEnableVertexAttribArray(0);

                if(nC > 0) {
                    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vSize));
                    glEnableVertexAttribArray(1);
                }

                if(nT > 0) {
                    // size: 
                    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vSize + cSize));
                    glEnableVertexAttribArray(2);
                }

            // unlock VBO & VAO
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_isLoaded = true;

        return true;
    }

    template <Dimension D>
    void Actor<D>::render(Matrices<D>* mvp) {
        if(!m_isLoaded || m_material.getShader() == 0)
            return;

        const bool hasTexture = m_material.getTextures().size() > 0;
        const bool translateOrRotate = m_hasToTranslate || m_hasToRotate;

        // apply matrix transformations
        // related to object's position / rotation
        if(translateOrRotate) {
            mvp->push();
            
            if(m_hasToTranslate)
                mvp->translate(this->m_position);

            if(m_hasToRotate)
                mvp->rotate(m_rotation);
        }

        // draw
        glPolygonMode(GL_FRONT_AND_BACK, m_material.isWireframed()? GL_LINE : m_polygonMode);
        glCullFace(m_cullFace);

        // lock shader
        m_material.getShader()->use();
            // lock VAO
            glBindVertexArray(m_VAO);

                // sends the matrices
                glUniformMatrix4fv(glGetUniformLocation(m_material.getShader()->getId(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp->get()));

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

        // then restore
        if(translateOrRotate)
            mvp->pop();
    }

}
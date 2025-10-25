/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Geometry.hpp"


namespace NoxEngine {

    Geometry::Geometry():
        m_hasLoaded(false),
        m_VAO(0), m_VBO(0), m_EBO(0),
        m_vertexCount(0), m_vElementCount(0)
    {}

    Geometry::Geometry(const Geometry& copy):
        Geometry()
    {
        m_VAO = copy.m_VAO;
        m_VBO = copy.m_VBO;
        m_EBO = copy.m_EBO;

        m_vertexCount = copy.m_vertexCount;
        m_vElementCount = copy.m_vElementCount;

        m_hasLoaded = copy.m_hasLoaded;
    }

    const Geometry& Geometry::operator=(const Geometry& copy) {
        m_VAO = copy.m_VAO;
        m_VBO = copy.m_VBO;
        m_EBO = copy.m_EBO;

        m_vertexCount = copy.m_vertexCount;
        m_vElementCount = copy.m_vElementCount;

        m_hasLoaded = copy.m_hasLoaded;

        return *this;
    }

    Geometry::~Geometry() {
        // TODO : Lifespan of the VAO/VBO/EBO depends of the lifespan of the geometry.
        //        It will be deleted, even if we copy the geometry, and the original is destroyed.
        /*deleteVAO();
        deleteVBO();
        deleteEBO();*/
    }

    bool Geometry::hasLoaded() const noexcept {
        return m_hasLoaded;
    }


    GLuint Geometry::getVAO() const  noexcept
    {
        return m_VAO;
    }

    GLuint Geometry::getEBO() const  noexcept
    {
        return m_EBO;
    }

    GLuint Geometry::getVertexCount() const noexcept {
        return m_vertexCount;
    }

    GLuint Geometry::getElementCount() const noexcept {
        return m_vElementCount;
    }

    void Geometry::deleteVBO() noexcept
    {
        if (glIsBuffer(m_VBO) == GL_TRUE)
            glDeleteBuffers(1, &m_VBO);
    }

    void Geometry::deleteVAO() noexcept
    {
        if (glIsVertexArray(m_VAO) == GL_TRUE)
            glDeleteVertexArrays(1, &m_VAO);
    }

    void Geometry::deleteEBO() noexcept
    {
        if (glIsBuffer(m_EBO) == GL_TRUE)
            glDeleteBuffers(1, &m_EBO);
    }

    bool Geometry::load(const GeometryData& data) {
        (void)data;
        return false;
    }

}
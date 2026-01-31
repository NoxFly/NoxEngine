/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "NoxEngine/Geometry/BufferGeometry.hpp"

#include <iostream>
#include <cstring>

namespace NoxEngine {

    std::shared_ptr<BufferGeometry> BufferGeometry::create() {
        return std::shared_ptr<BufferGeometry>(new BufferGeometry());
    }

    
    static inline size_t bytes_of_floats(size_t n) {
        return n * sizeof(GLfloat);
    }

    static inline size_t bytes_of_uints(size_t n) {
        return n * sizeof(unsigned int);
    }


    BufferGeometry::~BufferGeometry() {
        // delete GL resources if uploaded
        if (m_uploaded) {
            deleteGPUResources();
        }
    }

    // move ctor/assign
    BufferGeometry::BufferGeometry(BufferGeometry&& o) noexcept {
        m_positions = std::move(o.m_positions);
        m_normals = std::move(o.m_normals);
        m_uvs = std::move(o.m_uvs);
        m_colors = std::move(o.m_colors);
        m_indices = std::move(o.m_indices);

        m_uploaded = o.m_uploaded;
        m_vao = o.m_vao; o.m_vao = 0;
        m_vboPositions = o.m_vboPositions; o.m_vboPositions = 0;
        m_vboNormals = o.m_vboNormals; o.m_vboNormals = 0;
        m_vboUVs = o.m_vboUVs; o.m_vboUVs = 0;
        m_vboColors = o.m_vboColors; o.m_vboColors = 0;
        m_ebo = o.m_ebo; o.m_ebo = 0;
        m_vertexCount = o.m_vertexCount; o.m_vertexCount = 0;
        m_elementCount = o.m_elementCount; o.m_elementCount = 0;

        o.m_uploaded = false;
    }

    BufferGeometry& BufferGeometry::operator=(BufferGeometry&& o) noexcept {
        if (this != &o) {
            // release previous
            if (m_uploaded) {
                deleteGPUResources();
            }

            m_positions = std::move(o.m_positions);
            m_normals = std::move(o.m_normals);
            m_uvs = std::move(o.m_uvs);
            m_colors = std::move(o.m_colors);
            m_indices = std::move(o.m_indices);

            m_uploaded = o.m_uploaded;
            m_vao = o.m_vao; o.m_vao = 0;
            m_vboPositions = o.m_vboPositions; o.m_vboPositions = 0;
            m_vboNormals = o.m_vboNormals; o.m_vboNormals = 0;
            m_vboUVs = o.m_vboUVs; o.m_vboUVs = 0;
            m_vboColors = o.m_vboColors; o.m_vboColors = 0;
            m_ebo = o.m_ebo; o.m_ebo = 0;
            m_vertexCount = o.m_vertexCount; o.m_vertexCount = 0;
            m_elementCount = o.m_elementCount; o.m_elementCount = 0;

            o.m_uploaded = false;
        }

        return *this;
    }

    void BufferGeometry::deleteGPUResources() noexcept {
        if (m_uploaded) {
            if (m_ebo && glIsBuffer(m_ebo) == GL_TRUE)
                glDeleteBuffers(1, &m_ebo);

            if (m_vboPositions && glIsBuffer(m_vboPositions) == GL_TRUE)
                glDeleteBuffers(1, &m_vboPositions);

            if (m_vboNormals && glIsBuffer(m_vboNormals) == GL_TRUE)
                glDeleteBuffers(1, &m_vboNormals);

            if (m_vboUVs && glIsBuffer(m_vboUVs) == GL_TRUE)
                glDeleteBuffers(1, &m_vboUVs);

            if (m_vboColors && glIsBuffer(m_vboColors) == GL_TRUE)
                glDeleteBuffers(1, &m_vboColors);

            if (m_vao && glIsVertexArray(m_vao) == GL_TRUE)
                glDeleteVertexArrays(1, &m_vao);

            m_uploaded = false;
        }
    }

    // setters
    void BufferGeometry::setPositions(const std::vector<float>& positions) {
        m_positions = positions;
        m_uploaded = false;
    }

    void BufferGeometry::setNormals(const std::vector<float>& normals) {
        m_normals = normals;
        m_uploaded = false;
    }

    void BufferGeometry::setUVs(const std::vector<float>& uvs) {
        m_uvs = uvs;
        m_uploaded = false;
    }

    void BufferGeometry::setColors(const std::vector<float>& colors) {
        m_colors = colors;
        m_uploaded = false;
    }

    void BufferGeometry::setIndices(const std::vector<uint>& indices) {
        m_indices = indices;
        m_uploaded = false;
    }


    // queries
    bool BufferGeometry::hasPositions() const noexcept {
        return !m_positions.empty();
    }

    bool BufferGeometry::hasNormals() const noexcept {
        return !m_normals.empty();
    }

    bool BufferGeometry::hasUVs() const noexcept {
        return !m_uvs.empty();
    }

    bool BufferGeometry::hasColors() const noexcept {
        return !m_colors.empty();
    }

    bool BufferGeometry::hasIndices() const noexcept {
        return !m_indices.empty();
    }



    void BufferGeometry::uploadToGPU() const noexcept {
        if (m_uploaded)
            return;

        if (m_positions.empty())
            return;

        // generate
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vboPositions);
        
        if (m_vao == 0 || m_vboPositions == 0) {
            m_uploaded = false;
            return;
        }

        if (hasIndices()) {
            glGenBuffers(1, &m_ebo);
        }

        if (hasNormals())
            glGenBuffers(1, &m_vboNormals);

        if (hasUVs())
            glGenBuffers(1, &m_vboUVs);

        if (hasColors())
            glGenBuffers(1, &m_vboColors);


        // bind VAO
        glBindVertexArray(m_vao);

        // positions
        glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bytes_of_floats(m_positions.size())), m_positions.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0); // layout 0
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
        m_vertexCount = static_cast<GLuint>(m_positions.size() / 3);

        // normals
        if (hasNormals()) {
            glBindBuffer(GL_ARRAY_BUFFER, m_vboNormals);
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bytes_of_floats(m_normals.size())), m_normals.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
        }

        // uvs
        if (hasUVs()) {
            glBindBuffer(GL_ARRAY_BUFFER, m_vboUVs);
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bytes_of_floats(m_uvs.size())), m_uvs.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
        }

        // colors
        if (hasColors()) {
            glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bytes_of_floats(m_colors.size())), m_colors.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
        }

        // indices
        if (hasIndices()) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(bytes_of_uints(m_indices.size())), m_indices.data(), GL_STATIC_DRAW);
            m_elementCount = static_cast<GLuint>(m_indices.size());
        }
        else {
            m_elementCount = 0;
        }

        // unbind array buffer (keep EBO bound to VAO)
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_uploaded = true;
    }

    void BufferGeometry::bind() const noexcept {
        if (!m_uploaded)
            uploadToGPU();
        
        if (m_vao && glIsVertexArray(m_vao) == GL_TRUE)
            glBindVertexArray(m_vao);
    }

    void BufferGeometry::draw() const noexcept {
        bind();

        if (m_elementCount > 0) {
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_elementCount), GL_UNSIGNED_INT, nullptr);
        }
        else if (m_vertexCount > 0) {
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertexCount));
        }

        // unbind VAO for safety
        glBindVertexArray(0);
    }

}
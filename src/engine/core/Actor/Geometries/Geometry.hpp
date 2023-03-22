#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <vector>
#include <string>
#include <GL/glew.h>


namespace NoxEngine {

    struct GeometryData {
        std::vector<GLfloat> vertices;	// 2x2 or 3x3
        std::vector<GLfloat> normals;	// 2x2 or 3x3
        std::vector<GLfloat> colors;	// 3x3
        std::vector<GLfloat> uvs;		// 2x2
        std::vector<GLushort> indices; // EBO
    };

    

    class Geometry {
        public:
            static void setObjectsPath(const std::string& objectsPath);

            explicit Geometry();
            Geometry(const Geometry& copy);
            const Geometry& operator=(const Geometry& copy);
            ~Geometry();

            const bool hasLoaded() const;

            const GLuint getVAO() const noexcept;
            const GLuint getEBO() const noexcept;

            const GLuint getVertexCount() const;
            const GLuint getElementCount() const;

        protected:
            static std::string m_objectsPath;

            virtual bool load(const GeometryData& data);

            void deleteVBO() noexcept;
            void deleteVAO() noexcept;
            void deleteEBO() noexcept;

            bool m_hasLoaded;
            GLuint m_VAO, m_VBO, m_EBO;
            GLuint m_vertexCount, m_vElementCount;
    };

}

#endif // GEOMETRY_HPP
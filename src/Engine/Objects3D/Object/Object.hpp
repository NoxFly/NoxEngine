#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>

#include "Object3D.hpp"
#include "utils.hpp"
#include "Material.hpp"
#include "Geometry.hpp"
#include "MatricesMVP.hpp"


class Object: public Object3D {
    public:
        Object(const Geometry& geometry, const Material& material);
        virtual ~Object();

        std::string getUUID();

        Geometry* getGeometry();
        Material* getMaterial();
        
        void setCullFace(const GLenum cullFace);

        virtual bool load();
        void render(MatricesMVP* mvp);

    protected:
        const std::string m_uuid = generateUUID();
        Geometry m_geometry;
        Material m_material;
        GLuint m_VBO, m_VAO;
        GLenum m_cullFace;
        size_t m_vertexNumber;
        glm::vec3 m_rotation;

        virtual void draw(MatricesMVP* mvp) {(void)mvp;};
};

#endif // OBJECT_HPP
#ifndef OBJECT_H
#define OBJECT_H

#include <string>

#include "Object3D.h"
#include "utils.h"
#include "Material.h"
#include "Geometry.h"
#include "MatricesMVP.h"


class Object: public Object3D {
    public:
        Object(const Geometry& geometry, const Material& material);
        virtual ~Object();

        std::string getUUID();

        Geometry* getGeometry();
        Material* getMaterial();

        virtual bool load();
        void render(MatricesMVP* mvp);

    protected:
        const std::string m_uuid = generateUUID();
        Geometry m_geometry;
        Material m_material;
        GLuint m_VBO, m_VAO;
        size_t m_vertexNumber;
        glm::vec3 m_rotation;

        virtual void draw(MatricesMVP* mvp) {(void)mvp;};
};

#endif // OBJECT_H
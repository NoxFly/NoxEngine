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
        Object(Geometry& geometry, Material& material):
            Object3D(0, 0, 0),
            m_geometry(geometry), m_material(material), m_VBO(0), m_VAO(0), m_vertexNumber(0),
            m_rotation(0, 0, 0), m_hasToTranslate(false), m_hasToRotate(false) {}

        virtual ~Object() {};

        std::string getUUID() { return m_uuid; }
    
        void setPosition(const glm::vec3& position) {
            Object3D::setPosition(position);
            m_hasToTranslate = m_position.x != 0 || m_position.y != 0 || m_position.y != 0;
        }

        const glm::vec3& getRotation() const {
            return m_rotation;
        }

        void setRotation(const float x, const float y, const float z) {
            setRotation(glm::vec3(x, y, z));
        }

        void setRotation(const glm::vec3& rotation) {
            m_rotation = rotation;
            m_hasToRotate = m_rotation.x != 0 || m_rotation.y != 0 || m_rotation.z != 0;
        }

        void render(MatricesMVP* mvp) {
            // apply matrix transformations
            // related to object's position / rotation
            if(m_hasToTranslate || m_hasToRotate) {
                mvp->push();
                
                if(m_hasToTranslate)
                    mvp->translate(m_position);

                if(m_hasToRotate)
                    mvp->rotate(m_rotation);
            }

            // draw
            draw(mvp);

            // then restore
            if(m_hasToTranslate || m_hasToRotate)
                mvp->pop();
        }

    protected:
        std::string m_uuid = generateUUID();
        Geometry m_geometry;
        Material m_material;
        GLuint m_VBO, m_VAO, m_vertexNumber;
        glm::vec3 m_rotation;

        virtual bool load() { return false; }
        virtual void draw(MatricesMVP* mvp) {(void)mvp;};

    private:
        bool m_hasToTranslate, m_hasToRotate;
};

#endif // OBJECT_H
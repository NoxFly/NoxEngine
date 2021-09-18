#ifndef OBJECT_3D_H
#define OBJECT_3D_H

#include <glm/glm.hpp>

class Object3D {
    public:
        Object3D();
        Object3D(float x, float y, float z);
        Object3D(glm::vec3& position);
        Object3D(glm::vec3 position);
        ~Object3D();

        const glm::vec3& getPosition() const;
        const glm::vec3& getRotation() const;

        void setPosition(const float x, const float y, const float z);
        void setPosition(const glm::vec3& position);
        void setRotation(const float x, const float y, const float z);
        void setRotation(const glm::vec3& rotation);

    protected:
        glm::vec3 m_position,  m_rotation;
        bool m_hasToTranslate, m_hasToRotate;
};

#endif // OBJECT_3D_H
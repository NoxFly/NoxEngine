#ifndef OBJECT_3D_H
#define OBJECT_3D_H

#include <glm/glm.hpp>

class Object3D {
    public:
        Object3D(): Object3D(0, 0, 0) {}
        Object3D(float x, float y, float z): Object3D(glm::vec3(x, y, z)) {}
        Object3D(glm::vec3& position): m_position(position) {}
        Object3D(glm::vec3 position): m_position(position) {}
        virtual ~Object3D() {};

        const glm::vec3& getPosition() const {
            return m_position;
        }

        void setPosition(const float x, const float y, const float z) {
            setPosition(glm::vec3(x, y, z));
        }

        void setPosition(const glm::vec3& position) {
            m_position = position;
        }

    protected:
        glm::vec3 m_position;
};

#endif // OBJECT_3D_H
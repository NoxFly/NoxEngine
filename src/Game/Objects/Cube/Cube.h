#ifndef CUBE_H
#define CUBE_H

#include <glm/glm.hpp>

#include "Drawable.h"

class Cube: public Drawable {
	public:
		Cube(int x, int y, int z, float size);
        Cube(glm::vec3 position, float size);
		~Cube();

        Cube& operator=(Cube const &copy);

        glm::vec3 getPosition() const;
        void setPosition(const glm::vec3 position);

        float getSize() const;
        void setSize(const float size);

        Cube();
    protected:
        Cube(glm::vec3 position, float size, bool addColors);

        void load();
        void load(bool addColors);

        glm::vec3 m_position;
        float m_size;
};

#endif // CUBE_H
#ifndef CUBE_H
#define CUBE_H

#include <glm/glm.hpp>

#include "geometry.h"
#include "Drawable.h"
#include "MatricesMVP.h"

class Cube: public Drawable {
	public:
		Cube(Geometry& geometry, Material& material);
		~Cube();

        Cube& operator=(Cube const &copy);

        void draw(MatricesMVP& MVP);

        void setSize(const float size);
        void setPosition(const glm::vec3 position);
        void setPosition(const int x, const int y, const int z);

        float getSize() const;
        glm::vec3 getPosition() const;

    protected:
        Cube();
        Cube(Geometry& geometry, Material& material, bool hasToLoad);
        Cube(bool hasToLoad);

        void load(Geometry& geometry, Material& material);

        glm::vec3 m_position;
        float m_size;
};

#endif // CUBE_H
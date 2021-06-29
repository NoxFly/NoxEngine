#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <glm/glm.hpp>

#include "Cube.h"
#include "Texture.h"

class Block: public Cube {
	public:
        Block();
		Block(int x, int y, int z, const std::string& blockName);
		Block(glm::vec3 position, const std::string& blockName);
		~Block();

        Block& operator=(Block const &copy);

    protected:
        static float size;

        glm::vec3 blockPosition;
};

#endif // BLOCK_H
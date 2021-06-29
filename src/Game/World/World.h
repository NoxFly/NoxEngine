#ifndef WORLD_H
#define WORLD_H

#include <memory>

#include "Object.h"
#include "Block.h"

class World {
	public:
		World();
		~World();

        void load();
        void update(MatricesMVP& MVP);
        void render(MatricesMVP& MVP);

    private:
        std::unique_ptr<Block> createBlock(glm::vec3& position, const std::string& blockName);
        std::unique_ptr<Block> createBlock(const int x, const int y, const int z, const std::string& blockName);

        std::vector<std::unique_ptr<Object>> m_objects;
};

#endif // WORLD_H
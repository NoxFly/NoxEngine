#ifndef WORLD_H
#define WORLD_H

#include <memory>

#include "IniSet.h"
#include "Object.h"
#include "Block.h"
#include "Octree.hpp"

enum TileStateFlag { NONE, PENDING };

struct TileData {
    GLuint blockId = 0;
    GLuint blockType = 0;
    glm::vec3 position = glm::vec3(0, 0, 0);
    std::unique_ptr<Block> block = nullptr;
    TileStateFlag flags = NONE;
};


class World {
	public:
		World(IniSet* config);
		~World();

        void load();
        void update(MatricesMVP& MVP);
        void render(MatricesMVP& MVP);
        void generate();

    private:
        std::unique_ptr<Block> createBlock(glm::vec3& position, const std::string& blockName);
        std::unique_ptr<Block> createBlock(const int x, const int y, const int z, const std::string& blockName);

        IniSet* m_config;
        GLuint m_chunkSize;
        GLuint m_chunkRange;
        std::vector<TileData*> m_objects;
        std::vector<TileData> m_map;
        Octree<TileData> m_octree;
};

#endif // WORLD_H
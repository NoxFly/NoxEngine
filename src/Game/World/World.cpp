#include "World.h"

#include <glm/gtc/noise.hpp>

World::World(IniSet* config):
    m_config(config),
    m_chunkSize(config->getIntValue("GAME", "chunk_size", 10)),
    m_chunkRange(config->getIntValue("GAME", "chunk_range", 1)),
    m_objects{},
    m_map{},
    m_octree({-100, -100, -100, 200, 200, 200}, (200 % m_chunkSize) * m_chunkSize)
{

}

World::~World() {

}


void World::load() {
    generate();
}

void World::update(MatricesMVP& MVP) {
    (void)MVP;

    auto tmpOldObj = m_objects;

    m_octree.clear();

    for(auto& tile : m_map)
        m_octree.push(tile.position, &tile);

    m_objects = m_octree.queryRange(glm::vec3(m_chunkSize/2, 0, m_chunkSize/2), m_chunkSize/2);

    for(auto& o : m_objects) {
        o->block = createBlock(o->position, "stonebrick_carved");
        o->flags = PENDING;
    }

    for(auto& o : tmpOldObj) {
        if(o->flags == NONE)
            o->block = nullptr;
    }
}

void World::render(MatricesMVP& MVP) {
    for(auto& o : m_objects) {
        o->block->draw(MVP);
        o->flags = NONE;
    }
}

std::unique_ptr<Block> World::createBlock(const int x, const int y, const int z, const std::string& blockName) {
    return std::make_unique<Block>(glm::vec3(x, y, z), blockName);
}

std::unique_ptr<Block> World::createBlock(glm::vec3& position, const std::string& blockName) {
    return std::make_unique<Block>(position, blockName);
}

void World::generate() {
    const int depth = m_chunkSize / 2;

    for(GLuint x=0; x < m_chunkSize; x++) {
        for(GLuint z=0; z < m_chunkSize; z++) {
            const int y = glm::perlin(glm::vec2((float)x / m_chunkSize, (float)z / m_chunkSize)) * depth;
            glm::vec3 position(x, y, z);
            GLuint id = m_map.size();
            
            m_map.push_back({
                id, 0,
                position,
                nullptr
            });
        }
    }
}
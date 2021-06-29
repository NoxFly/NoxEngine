#include "World.h"

World::World():
    m_objects{}
{

}

World::~World() {

}


void World::load() {
    m_objects.push_back(createBlock(0, 0, 0, "grass_bottom"));
}

void World::update(MatricesMVP& MVP) {
    (void)MVP;
}

void World::render(MatricesMVP& MVP) {
    for(const auto& o : m_objects)
        o->draw(MVP);
}

std::unique_ptr<Block> World::createBlock(const int x, const int y, const int z, const std::string& blockName) {
    return std::make_unique<Block>(glm::vec3(x, y, z), blockName);
}

std::unique_ptr<Block> World::createBlock(glm::vec3& position, const std::string& blockName) {
    return std::make_unique<Block>(position, blockName);
}
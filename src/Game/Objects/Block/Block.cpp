#include "Block.h"

float Block::size = 1.0;

Block::Block():
    Cube()
{

}

Block::Block(int x, int y, int z, const std::string& blockName):
    Block(glm::vec3(x, y, z), blockName)
{

}

Block::Block(glm::vec3 position, const std::string& blockName):
    Cube(position, Block::size, false)
{
    setTexture(blockName);
    setShader("texture");

    m_geometry.texturesSize = 72;

    float textures[] = {
        0, 0,   1, 0,   1, 1,
        0, 0,   0, 1,   1, 1,

        0, 0,   1, 0,   1, 1,
        0, 0,   0, 1,   1, 1,

        0, 0,   1, 0,   1, 1,
        0, 0,   0, 1,   1, 1,

        0, 0,   1, 0,   1, 1,
        0, 0,   0, 1,   1, 1,

        0, 0,   1, 0,   1, 1,
        0, 0,   0, 1,   1, 1,

        0, 0,   1, 0,   1, 1,
        0, 0,   0, 1,   1, 1
    };

    m_geometry.textures = textures;

    Cube::load();
}

Block::~Block() {

}

Block& Block::operator=(Block const &copy) {
    Cube::operator=(copy);
    return *this;
}
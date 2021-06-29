#include "Block.h"

#include <vector>

float Block::size = 1.0;

Block::Block():
    Cube(),
    blockPosition(0, 0, 0)
{

}

Block::Block(int x, int y, int z, const std::string& blockName):
    Block(glm::vec3(x, y, z), blockName)
{

}

Block::Block(glm::vec3 position, const std::string& blockName):
    Cube(false),
    blockPosition(position)
{
    setSize(Block::size);
    setPosition(position * Block::size);

    std::vector<float> textures = {
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

    Geometry g;
    g.textures = { 72, textures };

    Material m;
    m.setShader("texture");
    m.setTextures(blockName);

    Cube::load(g, m);
}

Block::~Block() {

}

Block& Block::operator=(Block const &copy) {
    Cube::operator=(copy);
    return *this;
}
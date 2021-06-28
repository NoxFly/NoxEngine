#include "Block.h"

#include <vector>

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
    Cube(false)
{
    setSize(Block::size);
    setPosition(position);

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

    // (void)blockName;

    // colors to debug new structure geometry / material
    /* std::vector<float> colors = {
        0.93, 0.93, 0.93,   0.93, 0.93, 0.93,   0.93, 0.93, 0.93,
        0.93, 0.93, 0.93,   0.93, 0.93, 0.93,   0.93, 0.93, 0.93,

        0.93, 0.93, 0.93,   0.93, 0.93, 0.93,   0.93, 0.93, 0.93,
        0.93, 0.93, 0.93,   0.93, 0.93, 0.93,   0.93, 0.93, 0.93,

        0.93, 0.93, 0.93,   0.93, 0.93, 0.93,   0.93, 0.93, 0.93,
        0.93, 0.93, 0.93,   0.93, 0.93, 0.93,   0.93, 0.93, 0.93,

        0.93, 0.93, 0.93,   0.93, 0.93, 0.93,   0.93, 0.93, 0.93,
        0.93, 0.93, 0.93,   0.93, 0.93, 0.93,   0.93, 0.93, 0.93,

        0.93, 0.93, 0.93,   0.93, 0.93, 0.93,   0.93, 0.93, 0.93,
        0.93, 0.93, 0.93,   0.93, 0.93, 0.93,   0.93, 0.93, 0.93,

        0.93, 0.93, 0.93,   0.93, 0.93, 0.93,   0.93, 0.93, 0.93,
        0.93, 0.93, 0.93,   0.93, 0.93, 0.93,   0.93, 0.93, 0.93,
    }; */

    Geometry g;
    // g.colors = { 108, colors };
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
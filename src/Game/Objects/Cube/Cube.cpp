#include "Cube.h"

Cube::Cube(int x, int y, int z, float size):
    m_position(x, y, z),
    m_size(size)
{
    load();
}

Cube::Cube(glm::vec3 position, float size):
    m_position(position),
    m_size(size)
{
    load();
}

Cube::~Cube() {

}


void Cube::load() {
    float size = m_size / 2;

    float vertices[] = {
        -size, -size, -size,   size, -size, -size,   size, size, -size,
        -size, -size, -size,   -size, size, -size,   size, size, -size,

        size, -size, size,   size, -size, -size,   size, size, -size,
        size, -size, size,   size, size, size,   size, size, -size,

        -size, -size, size,   size, -size, size,   size, -size, -size,
        -size, -size, size,   -size, -size, -size,   size, -size, -size,

        -size, -size, size,   size, -size, size,   size, size, size,
        -size, -size, size,   -size, size, size,   size, size, size,

        -size, -size, -size,   -size, -size, size,   -size, size, size,
        -size, -size, -size,   -size, size, -size,   -size, size, size,

        -size, size, size,   size, size, size,   size, size, -size,
        -size, size, size,   -size, size, -size,   size, size, -size
    };

    float colors[] = {
        1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,

        0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,

        0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,

        1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,

        0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,

        0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0
    };

    m_geometry.vertices = vertices;
    m_geometry.colors = colors;
    m_geometry.elementCount = 108;

    Drawable::load();
}
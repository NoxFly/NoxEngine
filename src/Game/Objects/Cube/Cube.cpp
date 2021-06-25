#include "Cube.h"

Cube::Cube(int x, int y, int z, float size):
    m_position(x, y, z),
    m_size(size)
{

}

Cube::Cube(glm::vec3 position, float size):
    m_position(position),
    m_size(size)
{

}

Cube::~Cube() {

}


void Cube::load() {
    float size = m_size / 2;

    float vertices[] = {
        -size, -size, -size,   size, -size, -size,   size, size, -size,     // Face 1
        -size, -size, -size,   -size, size, -size,   size, size, -size,     // Face 1

        size, -size, size,   size, -size, -size,   size, size, -size,       // Face 2
        size, -size, size,   size, size, size,   size, size, -size,         // Face 2

        -size, -size, size,   size, -size, size,   size, -size, -size,      // Face 3
        -size, -size, size,   -size, -size, -size,   size, -size, -size,    // Face 3

        -size, -size, size,   size, -size, size,   size, size, size,        // Face 4
        -size, -size, size,   -size, size, size,   size, size, size,        // Face 4

        -size, -size, -size,   -size, -size, size,   -size, size, size,     // Face 5
        -size, -size, -size,   -size, size, -size,   -size, size, size,     // Face 5

        -size, size, size,   size, size, size,   size, size, -size,         // Face 6
        -size, size, size,   -size, size, -size,   size, size, -size        // Face 6
    };


    float colors[] = {
        1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 1
        1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 1

        0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 2
        0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 2

        0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 3
        0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 3

        1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 4
        1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 4

        0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 5
        0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 5

        0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 6
        0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0            // Face 6
    }; 

    Drawable::load(vertices, colors, 108);
}
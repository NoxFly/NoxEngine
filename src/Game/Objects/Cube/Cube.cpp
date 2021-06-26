#include "Cube.h"

Cube::Cube():
    Drawable(),
    m_position(0, 0, 0),
    m_size(0)
{

}

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

Cube::Cube(glm::vec3 position, float size, bool addColors):
    m_position(position),
    m_size(size)
{
    load(addColors);
}

Cube::~Cube() {

}


Cube& Cube::operator=(Cube const &copy) {
    m_position = copy.getPosition();
    m_size = copy.getSize();

    Drawable::operator=(copy);

    load(false);

    return *this;
}


void Cube::load() {
    load(true);
}

void Cube::load(bool addColors) {
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

    if(addColors) {
        float colors[] = {
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
        };

        m_geometry.colors = colors;
    }

    m_geometry.vertices = vertices;
    m_geometry.verticesSize = 108;

    Drawable::load();
}

glm::vec3 Cube::getPosition() const {
    return m_position;
}

void Cube::setPosition(const glm::vec3 position) {
    m_position = position;
}


float Cube::getSize() const {
    return m_size;
}

void Cube::setSize(const float size) {
    m_size = size;
}
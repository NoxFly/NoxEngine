#include "Cube.h"

#include <vector>

Cube::Cube():
    Drawable(),
    m_position(0, 0, 0),
    m_size(0)
{

}

Cube::Cube(Geometry& geometry, Material& material):
    Drawable(),
    m_position(0, 0, 0),
    m_size(0)
{
    load(geometry, material);
}


Cube::Cube(Geometry& geometry, Material& material, bool hasToLoad):
    Drawable(),
    m_position(0, 0, 0),
    m_size(0)
{
    if(hasToLoad)
        load(geometry, material);
}

Cube::Cube(bool hasToLoad):
    Drawable(),
    m_position(0, 0, 0),
    m_size(0)
{
    if(hasToLoad)
        load(m_geometry, m_material);
}

Cube::~Cube() {

}


Cube& Cube::operator=(Cube const &copy) {
    m_position = copy.getPosition();
    m_size = copy.getSize();

    Drawable::operator=(copy);

    load(m_geometry, m_material);

    return *this;
}

void Cube::load(Geometry& geometry, Material& material) {
    float s = m_size / 2;

    std::vector<float> vertices = {
        -s, -s, -s,     s, -s, -s,      s, s, -s,
        -s, -s, -s,     -s, s, -s,      s, s, -s,

        s, -s, s,       s, -s, -s,      s, s, -s,
        s, -s, s,       s, s, s,        s, s, -s,

        -s, -s, s,      s, -s, s,       s, -s, -s,
        -s, -s, s,      -s, -s, -s,     s, -s, -s,

        -s, -s, s,      s, -s, s,       s, s, s,
        -s, -s, s,      -s, s, s,       s, s, s,

        -s, -s, -s,     -s, -s, s,      -s, s, s,
        -s, -s, -s,     -s, s, -s,      -s, s, s,

        -s, s, s,       s, s, s,        s, s, -s,
        -s, s, s,       -s, s, -s,      s, s, -s
    };

    m_geometry = geometry;
    m_material = material;

    m_geometry.vertices = { 108, vertices };

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

void Cube::draw(MatricesMVP& MVP) {
    if(!m_material.hasShader())
        return;

    MVP.push();
        MVP.translate(m_position);
        Drawable::draw(MVP);
    MVP.pop();
}
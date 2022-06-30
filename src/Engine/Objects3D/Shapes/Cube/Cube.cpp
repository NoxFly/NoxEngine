#include "Cube.hpp"

#include "BasicMaterial.hpp"
#include "BoxGeometry.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

Cube::Cube(const float size):
    Mesh(BoxGeometry(size, size, size), BasicMaterial())
{

}

Cube::Cube(const float size, const Color& color):
    Mesh(BoxGeometry(size, size, size), BasicMaterial(color))
{

}

Cube::Cube(const float size, const std::string& textureName):
    Mesh(BoxGeometry(size, size, size), BasicMaterial(textureName))
{

}

Cube::Cube(const float size, const std::string& textureName, const Color& color):
    Mesh(BoxGeometry(size, size, size), BasicMaterial(color, textureName))
{

}


Cube::~Cube() {

}
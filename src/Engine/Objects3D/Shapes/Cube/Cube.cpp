#include "Cube.h"

#include "BasicMaterial.h"
#include "BoxGeometry.h"
#include "Shader.h"
#include "Texture.h"

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
#include "Cube.h"

#include "Material.h"
#include "BoxGeometry.h"
#include "Shader.h"
#include "Texture.h"

Cube::Cube(const float size):
    Mesh(BoxGeometry(size, size, size), Material())
{

}

Cube::Cube(const float size, const Color& color):
    Mesh(BoxGeometry(size, size, size), Material(color))
{

}

Cube::Cube(const float size, const std::string& shaderName):
    Mesh(BoxGeometry(size, size, size), Material(Shader::get(shaderName)))
{

}

Cube::Cube(const float size, const std::string& shaderName, const std::string& textureName):
    Mesh(BoxGeometry(size, size, size), Material(Shader::get(shaderName), Texture::get(textureName)))
{

}

Cube::Cube(const float size, const std::string& shaderName, const std::string& textureName, const Color& color):
    Mesh(BoxGeometry(size, size, size), Material(Shader::get(shaderName), Texture::get(textureName), color))
{

}

Cube::Cube(const float size, const std::string& shaderName, const Color& color):
    Mesh(BoxGeometry(size, size, size), Material(Shader::get(shaderName), color))
{

}


Cube::~Cube() {

}
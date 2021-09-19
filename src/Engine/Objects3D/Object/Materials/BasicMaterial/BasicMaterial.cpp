#include "BasicMaterial.h"

#include "Shader.h"
#include "Texture.h"

BasicMaterial::BasicMaterial():
    Material(Shader::get("color3D"))
{

}

BasicMaterial::BasicMaterial(const Color& color):
    Material(Shader::get("color3D"), color)
{

}

BasicMaterial::BasicMaterial(const std::string& textureName):
    Material(Shader::get("color3D"), Texture::get(textureName))
{

}

BasicMaterial::BasicMaterial(const Color& color, const std::string& textureName):
    Material(Shader::get("color3D"), Texture::get(textureName), color)
{

}

BasicMaterial::~BasicMaterial() {

}
#include "BasicMaterial.hpp"

#include "Shader.hpp"
#include "Texture.hpp"
#include "Console.hpp"


namespace NoxEngine {

    BasicMaterial::BasicMaterial():
        Material(Shader::get("color3D"))
    {}

    BasicMaterial::BasicMaterial(const Color& color):
        Material(Shader::get("color3D"), color)
    {}

    BasicMaterial::BasicMaterial(const std::string& textureName):
        Material(Shader::get("texture"), Texture::get(textureName))
    {}

    BasicMaterial::BasicMaterial(const Color& color, const std::string& textureName):
        Material(Shader::get("texture"), Texture::get(textureName), color)
    {}

    BasicMaterial::~BasicMaterial() {}

}
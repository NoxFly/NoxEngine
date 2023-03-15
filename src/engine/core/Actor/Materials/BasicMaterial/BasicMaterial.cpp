#include "BasicMaterial.hpp"

#include "core/Actor/Shader/Shader.hpp"
#include "core/Actor/Texture/Texture.hpp"
#include "Console/Console.hpp"


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
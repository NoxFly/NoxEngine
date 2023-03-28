#include "Basic2DMaterial.hpp"

#include "core/Actor/Shader/Shader.hpp"
#include "core/Actor/Texture/Texture.hpp"
#include "Console/Console.hpp"


namespace NoxEngine {

    Basic2DMaterial::Basic2DMaterial():
        Material(Shader::get("color2D"))
    {}

    Basic2DMaterial::Basic2DMaterial(const Color& color):
        Material(Shader::get("color2D"), color)
    {}

    Basic2DMaterial::Basic2DMaterial(const std::string& textureName):
        Material(Shader::get("texture"), Texture::get(textureName))
    {}

    Basic2DMaterial::Basic2DMaterial(const Color& color, const std::string& textureName):
        Material(Shader::get("texture"), Texture::get(textureName), color)
    {}

    Basic2DMaterial::~Basic2DMaterial() {}

}
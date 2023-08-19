#include "Basic3DMaterial.hpp"

#include "core/Actor/Shader/Shader.hpp"
#include "core/Actor/Texture/Texture.hpp"
#include "Console/Console.hpp"


namespace NoxEngine {

    Basic3DMaterial::Basic3DMaterial():
        Material(Shader::get("color3D"))
    {}

    Basic3DMaterial::Basic3DMaterial(const Color& color):
        Material(Shader::get("color3D"), color)
    {
        std::cout << "Basic3DMaterial constructor" << std::endl;
    }

    Basic3DMaterial::Basic3DMaterial(const std::string& textureName):
        Material(Shader::get("texture"), Texture::get(textureName))
    {}

    Basic3DMaterial::Basic3DMaterial(const Color& color, const std::string& textureName):
        Material(Shader::get("texture"), Texture::get(textureName), color)
    {}

    Basic3DMaterial::~Basic3DMaterial() {}

}
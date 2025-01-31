/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Basic3DMaterial.hpp"

#include "core/Actor/Shader/Shader.hpp"
#include "core/Actor/Texture/Texture.hpp"
#include "Console/Console.hpp"


namespace NoxEngine {

    Basic3DMaterial::Basic3DMaterial():
        Material(Shader::get("basic3D"))
    {}

    Basic3DMaterial::Basic3DMaterial(const Color& color):
        Material(Shader::get("basic3D"), color)
    {}
    
    // OUTDATED
    Basic3DMaterial::Basic3DMaterial(const std::string& textureName):
        Material(Shader::get("texture"), Texture::get(textureName))
    {}

    // OUTDATED
    Basic3DMaterial::Basic3DMaterial(const Color& color, const std::string& textureName):
        Material(Shader::get("texture"), Texture::get(textureName), color)
    {}

    Basic3DMaterial::~Basic3DMaterial() {}

}
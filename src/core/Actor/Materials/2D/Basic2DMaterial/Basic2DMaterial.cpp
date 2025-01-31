/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Basic2DMaterial.hpp"

#include "core/Actor/Shader/Shader.hpp"
#include "core/Actor/Texture/Texture.hpp"
#include "Console/Console.hpp"


namespace NoxEngine {

    Basic2DMaterial::Basic2DMaterial():
        Material(Shader::get("basic2D"))
    {}

    Basic2DMaterial::Basic2DMaterial(const Color& color):
        Material(Shader::get("basic2D"), color)
    {}

    // OUTDATED
    Basic2DMaterial::Basic2DMaterial(const std::string& textureName):
        Material(Shader::get("texture"), Texture::get(textureName))
    {}

    // OUTDATED
    Basic2DMaterial::Basic2DMaterial(const Color& color, const std::string& textureName):
        Material(Shader::get("texture"), Texture::get(textureName), color)
    {}

    Basic2DMaterial::~Basic2DMaterial() {}

}
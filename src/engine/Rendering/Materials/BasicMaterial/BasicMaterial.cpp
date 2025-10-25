/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "BasicMaterial.hpp"

#include "engine/Rendering/Shader/Shader.hpp"
#include "engine/Rendering/Texture/Texture.hpp"
#include "Console/Console.hpp"


namespace NoxEngine {

    BasicMaterial::BasicMaterial():
        Material(Shader::get("basic3D"))
    {}

    BasicMaterial::BasicMaterial(const Color& color):
        Material(Shader::get("basic3D"), color)
    {}
    
    // OUTDATED
    BasicMaterial::BasicMaterial(const std::string& textureName):
        Material(Shader::get("texture"), Texture::get(textureName))
    {}

}
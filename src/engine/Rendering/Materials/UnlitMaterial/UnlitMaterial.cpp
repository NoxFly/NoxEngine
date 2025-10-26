/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "UnlitMaterial.hpp"

#include "engine/Loaders/ShaderLoader/ShaderLoader.hpp"
#include "engine/Loaders/TextureLoader/TextureLoader.hpp"


namespace NoxEngine {

    UnlitMaterial::UnlitMaterial():
        Material(ShaderLoader::get("unlit"))
    {}

    UnlitMaterial::UnlitMaterial(const Color& color):
        Material(ShaderLoader::get("unlit"), color)
    {}
    
    // OUTDATED
    UnlitMaterial::UnlitMaterial(const std::string& textureName):
        Material(ShaderLoader::get("unlit"), TextureLoader::get(textureName))
    {}

}
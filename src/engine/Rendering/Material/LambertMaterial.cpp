/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "NoxEngine/Material/LambertMaterial.hpp"
#include "NoxEngine/Loader/ShaderLoader.hpp"
#include "NoxEngine/Loader/TextureLoader.hpp"

namespace NoxEngine {

    LambertMaterial::LambertMaterial():
        Material(ShaderLoader::get("lambert"))
    {}

    LambertMaterial::LambertMaterial(const Color& color):
        Material(ShaderLoader::get("lambert"), color)
    {}

    LambertMaterial::LambertMaterial(const std::string& textureName):
        Material(ShaderLoader::get("lambert"), TextureLoader::get(textureName))
    {}

}
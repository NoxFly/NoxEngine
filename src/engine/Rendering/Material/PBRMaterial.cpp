/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "NoxEngine/Material/PBRMaterial.hpp"
#include "NoxEngine/Loader/ShaderLoader.hpp"
#include "NoxEngine/Loader/TextureLoader.hpp"

namespace NoxEngine {

    PBRMaterial::PBRMaterial():
        Material(ShaderLoader::get("PBR"))
    {}

    PBRMaterial::PBRMaterial(const Color& color):
        Material(ShaderLoader::get("PBR"), color)
    {}

    PBRMaterial::PBRMaterial(const std::string& textureName):
        Material(ShaderLoader::get("PBR"), TextureLoader::get(textureName))
    {}

}
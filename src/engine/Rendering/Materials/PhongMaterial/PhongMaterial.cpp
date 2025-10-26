/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "PhongMaterial.hpp"

#include "engine/Loaders/ShaderLoader/ShaderLoader.hpp"
#include "engine/Loaders/TextureLoader/TextureLoader.hpp"

namespace NoxEngine {

    PhongMaterial::PhongMaterial():
        Material(ShaderLoader::get("phong"))
    {}

    PhongMaterial::PhongMaterial(const Color& color):
        Material(ShaderLoader::get("phong"), color)
    {}

    PhongMaterial::PhongMaterial(const std::string& textureName):
        Material(ShaderLoader::get("phong"), TextureLoader::get(textureName))
    {}

}
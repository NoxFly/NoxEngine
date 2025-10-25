/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "PhongMaterial.hpp"

#include "engine/Rendering/Shader/Shader.hpp"
#include "engine/Rendering/Texture/Texture.hpp"
#include "Console/Console.hpp"

// BUG : Shape is black no matter the light's properties and the object's color
namespace NoxEngine {

    PhongMaterial::PhongMaterial():
        Material(Shader::get("lightPhong3D"))
    {}

    PhongMaterial::PhongMaterial(const Color& color):
        Material(Shader::get("lightPhong3D"), color)
    {}

    PhongMaterial::PhongMaterial(const std::string& textureName):
        Material(Shader::get("lightPhong3D"), Texture::get(textureName))
    {}

}
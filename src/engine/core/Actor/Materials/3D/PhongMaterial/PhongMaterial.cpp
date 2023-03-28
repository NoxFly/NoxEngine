#include "PhongMaterial.hpp"

#include "core/Actor/Shader/Shader.hpp"
#include "core/Actor/Texture/Texture.hpp"
#include "Console/Console.hpp"


namespace NoxEngine {

    PhongMaterial::PhongMaterial():
        Material(Shader::get("lightPhong3D")),
        m_specularIntensity(0)
    {}

    PhongMaterial::PhongMaterial(const Color& color):
        Material(Shader::get("lightPhong3D"), color),
        m_specularIntensity(0)
    {}

    PhongMaterial::PhongMaterial(const std::string& textureName):
        Material(Shader::get("lightPhong3D"), Texture::get(textureName)),
        m_specularIntensity(0)
    {}

    PhongMaterial::PhongMaterial(const Color& color, const std::string& textureName):
        Material(Shader::get("lightPhong3D"), Texture::get(textureName), color),
        m_specularIntensity(0)
    {}

    PhongMaterial::~PhongMaterial() {}


    float PhongMaterial::getSpecular() const noexcept {
        return m_specularIntensity;
    }

    void PhongMaterial::setSpecular(const float intensity) noexcept {
        m_specularIntensity = intensity;
    }
    
}
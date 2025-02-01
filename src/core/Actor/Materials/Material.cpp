/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Material.hpp"

namespace NoxEngine {

    Material::Material():
        Material(nullptr, std::vector<Texture*>{}, Color())
    {}

    Material::Material(Shader* shader):
        Material(shader, std::vector<Texture*>{}, Color())
    {}

    Material::Material(Shader* shader, Texture* texture):
        Material(shader, std::vector<Texture*>{ texture }, Color())
    {
        m_textureAndColorOpacity.y = 0.f;
    }

    Material::Material(Shader* shader, Texture* texture, const Color& color):
        Material(shader, std::vector<Texture*>{ texture }, color)
    {}

    Material::Material(Shader* shader, const Color& color):
        Material(shader, std::vector<Texture*>{}, color)
    {}

    Material::Material(Shader* shader, const std::vector<Texture*>& textures):
        Material(shader, textures, Color())
    {}

    Material::Material(Shader* shader, const std::vector<Texture*>& textures, const Color& color):
        m_shader(shader),
        m_textures(textures),
        m_color(color),
        m_wireframe(false),
        m_textureAndColorOpacity(textures.size() == 0 ? 0 : 1, 1)
    {}

    Material::Material(Texture* texture):
        Material(nullptr, std::vector<Texture*>{ texture }, Color())
    {
        m_textureAndColorOpacity.y = 0.f;
    }

    Material::Material(const std::vector<Texture*>& textures):
        Material(nullptr, textures, Color())
    {
        m_textureAndColorOpacity.y = 0.f;
    }

    Material::Material(const Color& color):
        Material(nullptr, std::vector<Texture*>{}, color)
    {}

    Material::Material(Texture* texture, const Color& color):
        Material(nullptr, std::vector<Texture*>{ texture }, color)
    {}

    Material::Material(const std::vector<Texture*>& textures, const Color& color):
        Material(nullptr, textures, color)
    {}

    Material::~Material()
    {}


    void Material::setShader(Shader* shader) {
        m_shader = shader;
    }

    void Material::setTexture(Texture* texture) {
        m_textures.clear();

        // avoid the shader to set the texture coord as a black one
        // so no matter the color put after, resulting as black.
        // putting the texture's alpha to 0, we let the color speak out
        if(texture != nullptr) {
            m_textures.push_back(texture);

            if(m_textureAndColorOpacity.x == 0.f) {
                m_textureAndColorOpacity.x = 1.f;
            }
        }
        else {
            m_textureAndColorOpacity.x = 0.f;
        }
    }

    void Material::setTextures(const std::vector<Texture*>& textures) {
        m_textures = textures;

        if(m_textures.size() == 0) {
            m_textureAndColorOpacity.x = 0.f;
        }
        else if(m_textureAndColorOpacity.x == 0.f) {
            m_textureAndColorOpacity.x = 1.f;
        }
    }

    void Material::setColor(Color& color) {
        m_color = color;
    }

    void Material::setWireframe(const bool isWireframe) {
        m_wireframe = isWireframe;
    }

    void Material::setColorOpacity(const float opacity) {
        m_textureAndColorOpacity.y = std::min(std::max(0.f, opacity), 1.f);
    }

    void Material::setTextureOpacity(const float opacity) {
        if(m_textures.size() == 0) {
            return;
        }

        m_textureAndColorOpacity.x = std::min(std::max(0.f, opacity), 1.f);
    }

    Shader* Material::getShader() const {
        return m_shader;
    }

    std::vector<Texture*> Material::getTextures() const {
        return m_textures;
    }

    Color Material::getColor() const {
        return m_color;
    }

    bool Material::isWireframed() const {
        return m_wireframe;
    }

    float Material::getColorOpacity() const {
        return m_textureAndColorOpacity.y;
    }

    float Material::getTextureOpacity() const {
        return m_textureAndColorOpacity.x;
    }

}
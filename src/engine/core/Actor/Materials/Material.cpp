#include "Material.hpp"


namespace NoxEngine {

    Material::Material():
        Material(0, std::vector<Texture*>{}, Color())
    {}

    Material::Material(Shader* shader):
        Material(shader, std::vector<Texture*>{}, Color())
    {}

    Material::Material(Shader* shader, Texture* texture):
        Material(shader, std::vector<Texture*>{ texture }, Color())
    {}

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
        m_wireframe(false)
    {}

    Material::Material(Texture* texture):
        Material(0, std::vector<Texture*>{ texture }, Color())
    {}

    Material::Material(const std::vector<Texture*>& textures):
        Material(0, textures, Color())
    {}

    Material::Material(const Color& color):
        Material(0, std::vector<Texture*>{}, color)
    {}

    Material::Material(Texture* texture, const Color& color):
        Material(0, std::vector<Texture*>{ texture }, color)
    {}

    Material::Material(const std::vector<Texture*>& textures, const Color& color):
        Material(0, textures, color)
    {}

    
    Material::~Material() {}



    void Material::setShader(Shader* shader) {
        m_shader = shader;
    }

    void Material::setTexture(Texture* texture) {
        m_textures.clear();
        m_textures.push_back(texture);
    }

    void Material::setTextures(const std::vector<Texture*>& textures) {
        m_textures = textures;
    }

    void Material::setColor(Color& color) {
        m_color = color;
    }

    void Material::setWireframe(const bool isWireframe) {
        m_wireframe = isWireframe;
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

}
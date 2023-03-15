#include "Material.hpp"


namespace NoxEngine {

    Material::Material():
        Material(0, std::vector<Texture*>{}, std::vector<Color>{})
    {}

    Material::Material(Shader* shader):
        Material(shader, std::vector<Texture*>{}, std::vector<Color>{})
    {}

    Material::Material(Shader* shader, Texture* texture):
        Material(shader, std::vector<Texture*>{ texture }, std::vector<Color>{})
    {}

    Material::Material(Shader* shader, Texture* texture, const Color& color):
        Material(shader, std::vector<Texture*>{ texture }, std::vector<Color>{ color })
    {}

    Material::Material(Shader* shader, Texture* texture, const std::vector<Color>& colors):
        Material(shader, std::vector<Texture*>{ texture }, std::vector<Color>{ colors })
    {}

    Material::Material(Shader* shader, const Color& color):
        Material(shader, std::vector<Texture*>{}, std::vector<Color>{ color })
    {}

    Material::Material(Shader* shader, const std::vector<Color>& colors):
        Material(shader, std::vector<Texture*>{}, colors)
    {}

    Material::Material(Shader* shader, const std::vector<Texture*>& textures):
        Material(shader, textures, std::vector<Color>{})
    {}

    Material::Material(Shader* shader, const std::vector<Texture*>& textures, const Color& color):
        Material(shader, textures, std::vector<Color>{ color })
    {}

    Material::Material(Shader* shader, const std::vector<Texture*>& textures, const std::vector<Color>& colors):
        m_shader(shader), m_textures{ textures }, m_colors{ colors }, m_wireframe(false)
    {}

    Material::Material(Texture* texture):
        Material(0, std::vector<Texture*>{ texture }, std::vector<Color>{})
    {}

    Material::Material(const std::vector<Texture*>& textures):
        Material(0, textures, std::vector<Color>{})
    {}

    Material::Material(const Color& color):
        Material(0, std::vector<Texture*>{}, std::vector<Color>{ color })
    {}

    Material::Material(const std::vector<Color>& colors):
        Material(0, std::vector<Texture*>{}, colors)
    {}

    Material::Material(Texture* texture, const Color& color):
        Material(0, std::vector<Texture*>{ texture }, std::vector<Color>{ color })
    {}

    Material::Material(Texture* texture, const std::vector<Color>& colors):
        Material(0, std::vector<Texture*>{ texture }, colors)
    {}

    Material::Material(const std::vector<Texture*>& textures, const Color& color):
        Material(0, textures, std::vector<Color>{ color })
    {}

    Material::Material(const std::vector<Texture*>& textures, const std::vector<Color>& colors):
        Material(0, textures, colors)
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
        m_colors.clear();
        m_colors.push_back(color);
    }

    void Material::setColors(const std::vector<Color>& colors) {
        m_colors = colors;
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

    std::vector<Color> Material::getColors() const {
        return m_colors;
    }

    bool Material::isWireframed() const {
        return m_wireframe;
    }

}
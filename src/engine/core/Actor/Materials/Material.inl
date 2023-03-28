namespace NoxEngine {

    template <Dimension D>
    Material<D>::Material():
        Material(nullptr, std::vector<Texture*>{}, Color())
    {}

    template <Dimension D>
    Material<D>::Material(Shader* shader):
        Material(shader, std::vector<Texture*>{}, Color())
    {}

    template <Dimension D>
    Material<D>::Material(Shader* shader, Texture* texture):
        Material(shader, std::vector<Texture*>{ texture }, Color())
    {}

    template <Dimension D>
    Material<D>::Material(Shader* shader, Texture* texture, const Color& color):
        Material(shader, std::vector<Texture*>{ texture }, color)
    {}

    template <Dimension D>
    Material<D>::Material(Shader* shader, const Color& color):
        Material(shader, std::vector<Texture*>{}, color)
    {}

    template <Dimension D>
    Material<D>::Material(Shader* shader, const std::vector<Texture*>& textures):
        Material(shader, textures, Color())
    {}

    template <Dimension D>
    Material<D>::Material(Shader* shader, const std::vector<Texture*>& textures, const Color& color):
        m_shader(shader),
        m_textures(textures),
        m_color(color),
        m_wireframe(false)
    {}

    template <Dimension D>
    Material<D>::Material(Texture* texture):
        Material(nullptr, std::vector<Texture*>{ texture }, Color())
    {}

    template <Dimension D>
    Material<D>::Material(const std::vector<Texture*>& textures):
        Material(nullptr, textures, Color())
    {}

    template <Dimension D>
    Material<D>::Material(const Color& color):
        Material(nullptr, std::vector<Texture*>{}, color)
    {}

    template <Dimension D>
    Material<D>::Material(Texture* texture, const Color& color):
        Material(nullptr, std::vector<Texture*>{ texture }, color)
    {}

    template <Dimension D>
    Material<D>::Material(const std::vector<Texture*>& textures, const Color& color):
        Material(nullptr, textures, color)
    {}

    template <Dimension D>
    Material<D>::~Material()
    {}

    template <Dimension D>
    void Material<D>::setTexture(Texture* texture) {
        m_textures.clear();
        m_textures.push_back(texture);
    }

    template <Dimension D>
    void Material<D>::setTextures(const std::vector<Texture*>& textures) {
        m_textures = textures;
    }

    template <Dimension D>
    void Material<D>::setColor(Color& color) {
        m_color = color;
    }

    template <Dimension D>
    void Material<D>::setWireframe(const bool isWireframe) {
        m_wireframe = isWireframe;
    }


    template <Dimension D>
    Shader* Material<D>::getShader() const {
        return m_shader;
    }

    template <Dimension D>
    std::vector<Texture*> Material<D>::getTextures() const {
        return m_textures;
    }

    template <Dimension D>
    Color Material<D>::getColor() const {
        return m_color;
    }

    template <Dimension D>
    bool Material<D>::isWireframed() const {
        return m_wireframe;
    }

}
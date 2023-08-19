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
    {
        m_textureAndColorOpacity.y = 0.f;
    }

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
        m_wireframe(false),
        m_textureAndColorOpacity(textures.size() == 0 ? 0 : 1, 1)
    {}

    template <Dimension D>
    Material<D>::Material(Texture* texture):
        Material(nullptr, std::vector<Texture*>{ texture }, Color())
    {
        m_textureAndColorOpacity.y = 0.f;
    }

    template <Dimension D>
    Material<D>::Material(const std::vector<Texture*>& textures):
        Material(nullptr, textures, Color())
    {
        m_textureAndColorOpacity.y = 0.f;
    }

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
    void Material<D>::setShader(Shader* shader) {
        m_shader = shader;
    }

    template <Dimension D>
    void Material<D>::setTexture(Texture* texture) {
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

    template <Dimension D>
    void Material<D>::setTextures(const std::vector<Texture*>& textures) {
        m_textures = textures;

        if(m_textures.size() == 0) {
            m_textureAndColorOpacity.x = 0.f;
        }
        else if(m_textureAndColorOpacity.x == 0.f) {
            m_textureAndColorOpacity.x = 1.f;
        }
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
    void Material<D>::setColorOpacity(const float opacity) {
        m_textureAndColorOpacity.y = std::min(std::max(0.f, opacity), 1.f);
    }

    template <Dimension D>
    void Material<D>::setTextureOpacity(const float opacity) {
        if(m_textures.size() == 0) {
            return;
        }

        m_textureAndColorOpacity.x = std::min(std::max(0.f, opacity), 1.f);
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

    template <Dimension D>
    float Material<D>::getColorOpacity() const {
        return m_textureAndColorOpacity.y;
    }

    template <Dimension D>
    float Material<D>::getTextureOpacity() const {
        return m_textureAndColorOpacity.x;
    }

}
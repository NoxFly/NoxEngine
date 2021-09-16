#ifndef MATERIAL_H
#define MATERIAL_H

#include <vector>

#include "Texture.h"
#include "Shader.h"
#include "Color.h"

class Material {
    public:
        Material();
        Material(Shader* shader);
        Material(Shader* shader, Texture* texture);
        Material(Shader* shader, Texture* texture, const Color& color);
        Material(Shader* shader, Texture* texture, const std::vector<Color> colors);
        Material(Shader* shader, const Color& color);
        Material(Shader* shader, const std::vector<Color> colors);
        Material(Shader* shader, const std::vector<Texture*> textures);
        Material(Shader* shader, const std::vector<Texture*> textures, const Color& color);
        Material(Shader* shader, const std::vector<Texture*> textures, const std::vector<Color> colors);
        Material(Texture* texture);
        Material(const std::vector<Texture*> textures);
        Material(const Color& color);
        Material(const std::vector<Color> colors);
        Material(Texture* texture, const Color& color);
        Material(Texture* texture, const std::vector<Color> colors);
        Material(const std::vector<Texture*> textures, const Color& color);
        Material(const std::vector<Texture*> textures, const std::vector<Color> colors);

        ~Material();

        void setShader(Shader* shader);
        void setTexture(Texture* texture);
        void setTextures(const std::vector<Texture*> m_textures);
        void setColor(Color color);
        void setColors(const std::vector<Color> colors);
        void setWireframe(const bool isWireframe);

        Shader* getShader() const;
        std::vector<Texture*> getTextures() const;
        std::vector<Color> getColors() const;
        bool isWireframed() const;

    private:
        Shader* m_shader;
        std::vector<Texture*> m_textures;
        std::vector<Color> m_colors;
        bool m_wireframe;
};

#endif // MATERIAL_H
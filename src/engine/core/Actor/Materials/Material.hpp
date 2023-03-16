#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <vector>

#include "core/Actor/Texture/Texture.hpp"
#include "core/Actor/Shader/Shader.hpp"
#include "utils/Color.hpp"


namespace NoxEngine {

    class Material {
        public:
            explicit Material();
            explicit Material(Shader* shader);
            explicit Material(Shader* shader, Texture* texture);
            explicit Material(Shader* shader, Texture* texture, const Color& color);
            explicit Material(Shader* shader, const Color& color);
            explicit Material(Shader* shader, const std::vector<Texture*>& textures);
            explicit Material(Shader* shader, const std::vector<Texture*>& textures, const Color& color);
            explicit Material(Texture* texture);
            explicit Material(const std::vector<Texture*>& textures);
            explicit Material(const Color& color);
            explicit Material(Texture* texture, const Color& color);
            explicit Material(const std::vector<Texture*>& textures, const Color& color);

            ~Material();

            void setShader(Shader* shader);
            void setTexture(Texture* texture);
            void setTextures(const std::vector<Texture*>& m_textures);
            void setColor(Color& color);
            void setWireframe(const bool isWireframe);

            Shader* getShader() const;
            std::vector<Texture*> getTextures() const;
            Color getColor() const;
            bool isWireframed() const;

        private:
            Shader* m_shader;
            std::vector<Texture*> m_textures;
            Color m_color;
            bool m_wireframe;
    };

}

#endif // MATERIAL_HPP
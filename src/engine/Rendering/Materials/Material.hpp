/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <vector>

#include "engine/core/engine.typedef.hpp"
#include "engine/Rendering/Texture/Texture.hpp"
#include "engine/Rendering/Shader/Shader.hpp"
#include "engine/core/MatricesMVP/Matrices.hpp"
#include "utils/Color.hpp"
#include "engine/core/Scene/Scene.hpp"


namespace NoxEngine {

    class Material {
        public:
            explicit Material();
            explicit Material(Texture* texture);
            explicit Material(const std::vector<Texture*>& textures);
            explicit Material(const Color& color);
            explicit Material(Texture* texture, const Color& color);
            explicit Material(const std::vector<Texture*>& textures, const Color& color);

            /*Material(const Material& copy) = delete;
            const Material& operator=(const Material& copy) = delete;*/

            ~Material() = default;


            void setShader(Shader* shader);
            void setTexture(Texture* texture);
            void setTextures(const std::vector<Texture*>& m_textures);
            void setColor(Color& color);
            void setWireframe(const bool isWireframe);
            void setColorOpacity(const float opacity);
            void setTextureOpacity(const float opacity);

            Shader* getShader() const;
            std::vector<Texture*> getTextures() const;
            Color getColor() const;
            bool isWireframed() const;
            float getColorOpacity() const;
            float getTextureOpacity() const;

            virtual void transferUniforms(Matrices& mvp, const Scene* scene) {
                (void)mvp;
                (void)scene;
            };

        protected:
            explicit Material(Shader* shader);
            explicit Material(Shader* shader, Texture* texture);
            explicit Material(Shader* shader, Texture* texture, const Color& color);
            explicit Material(Shader* shader, const Color& color);
            explicit Material(Shader* shader, const std::vector<Texture*>& textures);
            explicit Material(Shader* shader, const std::vector<Texture*>& textures, const Color& color);

            Shader* m_shader;
            std::vector<Texture*> m_textures;
            Color m_color;
            bool m_wireframe;
            V2D m_textureAndColorOpacity;
    };

}

#endif // MATERIAL_HPP
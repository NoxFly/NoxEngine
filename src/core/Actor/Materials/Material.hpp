#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <vector>

#include "core/engine.typedef.hpp"
#include "core/Actor/Texture/Texture.hpp"
#include "core/Actor/Shader/Shader.hpp"
#include "core/MatricesMVP/Matrices.hpp"
#include "utils/Color.hpp"
#include "core/Scene/Scene.hpp"


namespace NoxEngine {

    template <Dimension D>
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

            virtual void transferUniforms(Matrices<D>& mvp, const Scene<D>* scene) {
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
    };

}

#include "Material.inl"

#endif // MATERIAL_HPP
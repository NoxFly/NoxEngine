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

    struct MaterialData {
        std::string name;
        Color diffuse {};
        Color ambient {};
        Color specular {};
        float shininess = 32.f;
    };

    class Material {
        public:
            static std::shared_ptr<Material> create();

            // Constructeurs
            explicit Material();
            explicit Material(Shader* shader);
            explicit Material(Shader* shader, const Color& color);
            explicit Material(Shader* shader, Texture* texture);
            explicit Material(Shader* shader, const std::vector<Texture*>& textures, const Color& color = Color(1.f,1.f,1.f));

            ~Material() = default;

            // Setters
            void setShader(Shader* shader) noexcept;
            void setTextures(const std::vector<Texture*>& textures) noexcept;
            void setDiffuse(const Color& c) noexcept;
            void setColor(const Color& color) noexcept;
            void setAmbient(const Color& ambient) noexcept;
            void setSpecular(const Color& specular) noexcept;
            void setShininess(float shininess) noexcept;
            void setWireframe(bool wireframe) noexcept;
            void setOpacity(float opacity) noexcept;

            // Getters
            Shader* getShader() const noexcept;
            const std::vector<Texture*>& getTextures() const noexcept;
            const Color& getDiffuse() const noexcept;
            const Color& getAmbient() const noexcept;
            const Color& getSpecular() const noexcept;
            float getShininess() const noexcept;
            bool isWireframed() const noexcept;
            float getOpacity() const noexcept;

            virtual void transferUniforms(Matrices& mvp, const Scene* scene) const;

        protected:
            Shader* m_shader{ nullptr };
            std::vector<Texture*> m_textures;
            Color m_diffuse{ 1.0f, 1.0f, 1.0f };        // Couleur diffuse
            Color m_ambient{ 0.2f, 0.2f, 0.2f };        // Couleur ambiante
            Color m_specular{ 1.0f, 1.0f, 1.0f };       // Couleur spéculaire
            float m_shininess{ 32.f };
            bool m_wireframe{ false };
            float m_opacity{ 1.f };
    };

}

#endif // MATERIAL_HPP
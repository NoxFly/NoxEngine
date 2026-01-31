/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <vector>

#include "NoxEngine/engine.typedef.hpp"
#include "NoxEngine/Camera/Matrices.hpp"
#include "NoxEngine/Rendering/Texture.hpp"
#include "NoxEngine/Rendering/Shader.hpp"
#include "NoxEngine/Rendering/Scene.hpp"
#include "NoxEngine/utils/Color.hpp"


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
            template<typename T, typename = std::enable_if_t<std::is_base_of_v<Material, T>>, typename... Args>
            static std::shared_ptr<T> create(Args&&... args) {
                return std::make_shared<T>(std::forward<Args>(args)...);
            }

            static std::shared_ptr<Material> create();
            
            static void setDefaultShader(std::shared_ptr<Shader> shader) noexcept;

            // Constructeurs
            explicit Material() = default;
            explicit Material(std::shared_ptr<Shader> shader);
            explicit Material(std::shared_ptr<Shader> shader, const Color& color);
            explicit Material(std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture);
            explicit Material(std::shared_ptr<Shader> shader, const std::vector<std::shared_ptr<Texture>>& textures, const Color& color = Color(1.f,1.f,1.f));

            ~Material() = default;

            // Setters
            void setShader(std::shared_ptr<Shader> shader) noexcept;
            void setTextures(const std::vector<std::shared_ptr<Texture>>& textures) noexcept;
            void setDiffuse(const Color& c) noexcept;
            void setColor(const Color& color) noexcept;
            void setAmbient(const Color& ambient) noexcept;
            void setSpecular(const Color& specular) noexcept;
            void setShininess(float shininess) noexcept;
            void setWireframe(bool wireframe) noexcept;
            void setOpacity(float opacity) noexcept;

            // Getters
            std::shared_ptr<Shader> getShader() const noexcept;
            const std::vector<std::shared_ptr<Texture>>& getTextures() const noexcept;
            const Color& getDiffuse() const noexcept;
            const Color& getAmbient() const noexcept;
            const Color& getSpecular() const noexcept;
            float getShininess() const noexcept;
            bool isWireframed() const noexcept;
            float getOpacity() const noexcept;

            virtual void transferUniforms(Matrices& mvp, const Scene* scene, const V3D& cameraPosition) const;

        protected:
            inline static std::shared_ptr<Shader> s_defaultShader { nullptr };

            std::shared_ptr<Shader> m_shader { nullptr };
            std::vector<std::shared_ptr<Texture>> m_textures;
            Color m_diffuse { 1.0f, 1.0f, 1.0f };        // Couleur diffuse
            Color m_ambient { 0.2f, 0.2f, 0.2f };        // Couleur ambiante
            Color m_specular { 1.0f, 1.0f, 1.0f };       // Couleur spéculaire
            float m_shininess { 32.f };
            bool m_wireframe { false };
            float m_opacity { 1.f };
    };

}

#endif // MATERIAL_HPP
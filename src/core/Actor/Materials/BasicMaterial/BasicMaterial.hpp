/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef BASIC_3D_MATERIAL_HPP
#define BASIC_3D_MATERIAL_HPP

#include "core/Actor/Materials/Material.hpp"
#include "core/MatricesMVP/Matrices.hpp"
#include "utils/Color.hpp"


namespace NoxEngine {

    class BasicMaterial: public Material {
        public:
            explicit BasicMaterial();
            explicit BasicMaterial(const Color& color);
            explicit BasicMaterial(const std::string& textureName);
            explicit BasicMaterial(const Color& color, const std::string& textureName);
            ~BasicMaterial();

            void transferUniforms(Matrices& mvp, const Scene* scene) override {
                (void)scene;
                m_shader->setMat4("MVP", mvp.get());
                m_shader->setVec3("objectColor", m_color.vec3());
                m_shader->setVec2("textureAndColorOpacity", m_textureAndColorOpacity);
            }
    };

}

#endif // BASIC_3D_MATERIAL_HPP
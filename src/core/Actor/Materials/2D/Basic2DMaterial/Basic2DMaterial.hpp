#ifndef BASIC_2D_MATERIAL_HPP
#define BASIC_2D_MATERIAL_HPP

#include "core/Actor/Materials/Material.hpp"
#include "core/MatricesMVP/Matrices.hpp"
#include "utils/Color.hpp"


namespace NoxEngine {

    class Basic2DMaterial: public Material<V2D> {
        public:
            explicit Basic2DMaterial();
            explicit Basic2DMaterial(const Color& color);
            explicit Basic2DMaterial(const std::string& textureName);
            explicit Basic2DMaterial(const Color& color, const std::string& textureName);
            ~Basic2DMaterial();

            void transferUniforms(Matrices<V2D>& mvp, const Scene<V2D>* scene) override {
                (void)scene;
                m_shader->setMat4("MVP", mvp.get());
                m_shader->setVec3("objectColor", m_color.vec3());
                m_shader->setVec2("textureAndColorOpacity", m_textureAndColorOpacity);
            }
    };

}

#endif // BASIC_2D_MATERIAL_HPP
#ifndef BASIC_3D_MATERIAL_HPP
#define BASIC_3D_MATERIAL_HPP

#include "core/Actor/Materials/Material.hpp"
#include "core/MatricesMVP/Matrices.hpp"
#include "utils/Color.hpp"


namespace NoxEngine {

    class Basic3DMaterial: public Material<V3D> {
        public:
            explicit Basic3DMaterial();
            explicit Basic3DMaterial(const Color& color);
            explicit Basic3DMaterial(const std::string& textureName);
            explicit Basic3DMaterial(const Color& color, const std::string& textureName);
            ~Basic3DMaterial();

            void transferUniforms(Matrices<V3D>& mvp, const Scene<V3D>* scene) override;
    };

}

#include "Basic3DMaterial.inl"

#endif // BASIC_3D_MATERIAL_HPP
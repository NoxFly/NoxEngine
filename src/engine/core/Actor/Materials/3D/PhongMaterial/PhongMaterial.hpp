#ifndef PHONG_MATERIAL_HPP
#define PHONG_MATERIAL_HPP

#include <vector>

#include "core/MatricesMVP/Matrices.hpp"
#include "core/Actor/Materials/Material.hpp"
#include "core/Actor/Light/Light.hpp"
#include "utils/Color.hpp"


namespace NoxEngine {

    class PhongMaterial : public Material<V3D> {
        public:
            explicit PhongMaterial();
            explicit PhongMaterial(const Color& color);
            explicit PhongMaterial(const std::string& textureName);
            explicit PhongMaterial(const Color& color, const std::string& textureName);
            ~PhongMaterial();

            void transferUniforms(Matrices<V3D>& mvp, const Scene<V3D>* scene) override;

            float getSpecular() const noexcept;

            void setSpecular(const float intensity) noexcept;

        protected:
            float m_specularIntensity;
    };

}

#include "PhongMaterial.inl"

#endif // PHONG_MATERIAL_HPP
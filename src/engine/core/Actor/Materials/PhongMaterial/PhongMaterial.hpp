#ifndef PHONG_MATERIAL_HPP
#define PHONG_MATERIAL_HPP

#include <vector>

#include "core/Actor/Materials/Material.hpp"
#include "core/MatricesMVP/Matrices.hpp"
#include "utils/Color.hpp"
#include "core/Actor/Light/Light.hpp"


namespace NoxEngine {

    class PhongMaterial : public Material {
        public:
            explicit PhongMaterial();
            explicit PhongMaterial(const Color& color);
            explicit PhongMaterial(const std::string& textureName);
            explicit PhongMaterial(const Color& color, const std::string& textureName);
            ~PhongMaterial();

            void transferUniforms(
                const M4& mvp, const M4& m, const M4& v, const M4& p,
                const std::vector<std::shared_ptr<Light>>& lights
            );

            float getSpecular() const;

            void setSpecular(const float intensity);

        protected:
            float m_specularIntensity;
    };

}

#include "PhongMaterial.inl"

#endif // PHONG_MATERIAL_HPP
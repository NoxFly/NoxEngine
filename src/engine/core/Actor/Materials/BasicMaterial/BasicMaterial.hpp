#ifndef BASIC_MATERIAL_HPP
#define BASIC_MATERIAL_HPP

#include "core/Actor/Materials/Material.hpp"
#include "core/MatricesMVP/Matrices.hpp"
#include "utils/Color.hpp"
#include "core/Actor/Light/Light.hpp"


namespace NoxEngine {

    class BasicMaterial: public Material {
        public:
            explicit BasicMaterial();
            explicit BasicMaterial(const Color& color);
            explicit BasicMaterial(const std::string& textureName);
            explicit BasicMaterial(const Color& color, const std::string& textureName);
            ~BasicMaterial();

            void transferUniforms(
                const M4& mvp, const M4& m, const M4& v, const M4& p,
                const std::vector<std::shared_ptr<Light>>& lights
            );
    };

}

#include "BasicMaterial.inl"

#endif // BASIC_MATERIAL_HPP
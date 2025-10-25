/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef PHONG_MATERIAL_HPP
#define PHONG_MATERIAL_HPP

#include <vector>

#include "engine/core/MatricesMVP/Matrices.hpp"
#include "engine/Rendering/Materials/Material.hpp"
#include "engine/Rendering/Light/Light.hpp"
#include "utils/Color.hpp"


namespace NoxEngine {

    class PhongMaterial : public Material {
        public:
            explicit PhongMaterial();
            explicit PhongMaterial(const Color& color);
            explicit PhongMaterial(const std::string& textureName);
            ~PhongMaterial() = default;
    };

}

#endif // PHONG_MATERIAL_HPP
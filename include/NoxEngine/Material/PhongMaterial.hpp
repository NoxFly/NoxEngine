/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef PHONG_MATERIAL_HPP
#define PHONG_MATERIAL_HPP

#include <vector>

#include "NoxEngine/Camera/Matrices.hpp"
#include "NoxEngine/Material/Material.hpp"
#include "NoxEngine/Light/Light.hpp"
#include "NoxEngine/utils/Color.hpp"


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
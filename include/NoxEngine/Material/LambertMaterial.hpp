/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef LAMBERT_MATERIAL_HPP
#define LAMBERT_MATERIAL_HPP

#include <vector>

#include "NoxEngine/Camera/Matrices.hpp"
#include "NoxEngine/Material/Material.hpp"
#include "NoxEngine/Light/Light.hpp"
#include "NoxEngine/utils/Color.hpp"


namespace NoxEngine {

    class LambertMaterial : public Material {
        public:
            explicit LambertMaterial();
            explicit LambertMaterial(const Color& color);
            explicit LambertMaterial(const std::string& textureName);
            ~LambertMaterial() = default;
    };

}

#endif // LAMBERT_MATERIAL_HPP
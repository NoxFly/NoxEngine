/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef LAMBERT_MATERIAL_HPP
#define LAMBERT_MATERIAL_HPP

#include <vector>

#include "engine/core/MatricesMVP/Matrices.hpp"
#include "engine/Rendering/Materials/Material.hpp"
#include "engine/Rendering/Light/Light.hpp"
#include "utils/Color.hpp"


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
/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef BASIC_3D_MATERIAL_HPP
#define BASIC_3D_MATERIAL_HPP

#include "engine/Rendering/Materials/Material.hpp"
#include "engine/core/MatricesMVP/Matrices.hpp"
#include "utils/Color.hpp"


namespace NoxEngine {

    class BasicMaterial: public Material {
        public:
            explicit BasicMaterial();
            explicit BasicMaterial(const Color& color);
            explicit BasicMaterial(const std::string& textureName);
            ~BasicMaterial() = default;
    };

}

#endif // BASIC_3D_MATERIAL_HPP
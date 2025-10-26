/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef PBR_MATERIAL_HPP
#define PBR_MATERIAL_HPP

#include <vector>

#include "engine/core/MatricesMVP/Matrices.hpp"
#include "engine/Rendering/Materials/Material.hpp"
#include "engine/Rendering/Light/Light.hpp"
#include "utils/Color.hpp"


namespace NoxEngine {

    class PBRMaterial : public Material {
        public:
            explicit PBRMaterial();
            explicit PBRMaterial(const Color& color);
            explicit PBRMaterial(const std::string& textureName);
            ~PBRMaterial() = default;
    };

}

#endif // PBR_MATERIAL_HPP
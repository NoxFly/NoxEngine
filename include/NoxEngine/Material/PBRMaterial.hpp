/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef PBR_MATERIAL_HPP
#define PBR_MATERIAL_HPP

#include <vector>

#include "NoxEngine/Camera/Matrices.hpp"
#include "NoxEngine/Material/Material.hpp"
#include "NoxEngine/Light/Light.hpp"
#include "NoxEngine/utils/Color.hpp"


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
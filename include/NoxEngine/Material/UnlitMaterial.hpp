/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef UNLIT_MATERIAL_HPP
#define UNLIT_MATERIAL_HPP

#include "NoxEngine/Material/Material.hpp"
#include "NoxEngine/Camera/Matrices.hpp"
#include "NoxEngine/utils/Color.hpp"


namespace NoxEngine {

    class UnlitMaterial: public Material {
        public:
            explicit UnlitMaterial();
            explicit UnlitMaterial(const Color& color);
            explicit UnlitMaterial(const std::string& textureName);
            ~UnlitMaterial() = default;
    };

}

#endif // UNLIT_MATERIAL_HPP
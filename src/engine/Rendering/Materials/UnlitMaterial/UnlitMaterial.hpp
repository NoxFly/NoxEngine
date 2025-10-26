/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef UNLIT_MATERIAL_HPP
#define UNLIT_MATERIAL_HPP

#include "engine/Rendering/Materials/Material.hpp"
#include "engine/core/MatricesMVP/Matrices.hpp"
#include "utils/Color.hpp"


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
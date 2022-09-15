#ifndef BASIC_MATERIAL_HPP
#define BASIC_MATERIAL_HPP

#include "Material.hpp"
#include "Color.hpp"


namespace NoxEngine {

    class BasicMaterial: public Material {
        public:
            explicit BasicMaterial();
            explicit BasicMaterial(const Color& color);
            explicit BasicMaterial(const std::string& textureName);
            explicit BasicMaterial(const Color& color, const std::string& textureName);
            ~BasicMaterial();
    };

}

#endif // BASIC_MATERIAL_HPP
#ifndef BASIC_MATERIAL_HPP
#define BASIC_MATERIAL_HPP

#include "Material.hpp"
#include "Color.hpp"

class BasicMaterial: public Material {
    public:
        BasicMaterial();
        BasicMaterial(const Color& color);
        BasicMaterial(const std::string& textureName);
        BasicMaterial(const Color& color, const std::string& textureName);
        ~BasicMaterial();
};

#endif // BASIC_MATERIAL_HPP
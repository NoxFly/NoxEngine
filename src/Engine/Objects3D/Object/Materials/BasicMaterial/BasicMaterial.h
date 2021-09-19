#ifndef BASIC_MATERIAL_H
#define BASIC_MATERIAL_H

#include "Material.h"
#include "Color.h"

class BasicMaterial: public Material {
    public:
        BasicMaterial();
        BasicMaterial(const Color& color);
        BasicMaterial(const std::string& textureName);
        BasicMaterial(const Color& color, const std::string& textureName);
        ~BasicMaterial();
};

#endif // BASIC_MATERIAL_H
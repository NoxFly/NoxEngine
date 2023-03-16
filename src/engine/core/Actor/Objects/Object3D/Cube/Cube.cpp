#include "Cube.hpp"

#include "core/Actor/Materials/BasicMaterial/BasicMaterial.hpp"
#include "core/Actor/Geometries/3D/BoxGeometry/BoxGeometry.hpp"
#include "core/Actor/Shader/Shader.hpp"
#include "core/Actor/Texture/Texture.hpp"
#include <iostream>

namespace NoxEngine {

    Cube::Cube(const float size):
        Object3D(BoxGeometry(size, size, size), BasicMaterial())
    {
        
    }

    Cube::Cube(const float size, const Color& color):
        Object3D(BoxGeometry(size, size, size), BasicMaterial(color))
    {

    }

    Cube::Cube(const float size, const std::string& textureName):
        Object3D(BoxGeometry(size, size, size), BasicMaterial(textureName))
    {

    }

    Cube::Cube(const float size, const std::string& textureName, const Color& color):
        Object3D(BoxGeometry(size, size, size), BasicMaterial(color, textureName))
    {

    }


    Cube::~Cube() {

    }

}
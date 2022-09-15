#include "Cube.hpp"

#include "BasicMaterial.hpp"
#include "BoxGeometry.hpp"
#include "Shader.hpp"
#include "Texture.hpp"


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
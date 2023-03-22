#include "Cube.hpp"

#include "core/Actor/Materials/PhongMaterial/PhongMaterial.hpp"
#include "core/Actor/Geometries/3D/BoxGeometry/BoxGeometry.hpp"

using namespace std;

namespace NoxEngine {

    Cube::Cube(const float size):
        Object3D(BoxGeometry(size, size, size), PhongMaterial())
    {
        
    }

    Cube::Cube(const float size, const Color& color):
        Object3D(BoxGeometry(size, size, size), PhongMaterial(color))
    {

    }

    Cube::Cube(const float size, const std::string& textureName) :
        Object3D(BoxGeometry(size, size, size), PhongMaterial(textureName))
    {

    }

    Cube::Cube(const float size, const std::string& textureName, const Color& color) :
        Object3D(BoxGeometry(size, size, size), PhongMaterial(color, textureName))
    {

    }


    Cube::~Cube() {

    }

}
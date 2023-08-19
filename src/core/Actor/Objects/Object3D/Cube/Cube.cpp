#include "Cube.hpp"

#include "core/Actor/Materials/3D/PhongMaterial/PhongMaterial.hpp"
#include "core/Actor/Materials/3D/Basic3DMaterial/Basic3DMaterial.hpp"
#include "core/Actor/Geometries/3D/BoxGeometry/BoxGeometry.hpp"

// TODO : change by PhongMaterial when bug fixed #3
namespace NoxEngine {

    Cube::Cube(const float size):
        Object3D(BoxGeometry(size, size, size), Basic3DMaterial())
    {
        
    }

    Cube::Cube(const float size, const Color& color):
        Object3D(BoxGeometry(size, size, size), Basic3DMaterial(color))
    {
        std::cout << "Cube constructor" << std::endl;
    }

    Cube::Cube(const float size, const std::string& textureName):
        Object3D(BoxGeometry(size, size, size), Basic3DMaterial(textureName))
    {

    }

    Cube::Cube(const float size, const std::string& textureName, const Color& color):
        Object3D(BoxGeometry(size, size, size), Basic3DMaterial(color, textureName))
    {

    }


    Cube::~Cube() {

    }

}
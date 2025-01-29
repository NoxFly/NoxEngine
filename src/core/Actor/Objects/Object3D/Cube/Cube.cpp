/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Cube.hpp"

#include "core/Actor/Materials/3D/PhongMaterial/PhongMaterial.hpp"
#include "core/Actor/Geometries/3D/BoxGeometry/BoxGeometry.hpp"


namespace NoxEngine {

    Cube::Cube(const float size):
        Object3D(new BoxGeometry(size, size, size), new PhongMaterial())
    {}

    Cube::Cube(const float size, const Color& color):
        Object3D(new BoxGeometry(size, size, size), new PhongMaterial(color))
    {}

    Cube::Cube(const float size, const std::string& textureName):
        Object3D(new BoxGeometry(size, size, size), new PhongMaterial(textureName))
    {}

    Cube::Cube(const float size, const std::string& textureName, const Color& color):
        Object3D(new BoxGeometry(size, size, size), new PhongMaterial(color, textureName))
    {}


    Cube::~Cube() {}

}
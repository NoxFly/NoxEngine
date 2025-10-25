/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Cube.hpp"

#include "engine/Rendering/Materials/PhongMaterial/PhongMaterial.hpp"
#include "engine/Rendering/Geometries/3D/BoxGeometry/BoxGeometry.hpp"


namespace NoxEngine {

    Cube::Cube(const float size):
        Mesh3D(new BoxGeometry(size, size, size), new PhongMaterial())
    {}

    Cube::Cube(const float size, const Color& color):
        Mesh3D(new BoxGeometry(size, size, size), new PhongMaterial(color))
    {}

    Cube::Cube(const float size, const std::string& textureName):
        Mesh3D(new BoxGeometry(size, size, size), new PhongMaterial(textureName))
    {}

    Cube::Cube(const float size, const std::string& textureName, const Color& color):
        Mesh3D(new BoxGeometry(size, size, size), new PhongMaterial(color, textureName))
    {}

}
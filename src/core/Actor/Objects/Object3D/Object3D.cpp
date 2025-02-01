/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Object3D.hpp"

#include <fstream>

#include "Console/Console.hpp"


namespace NoxEngine {

    Object3D::Object3D():
        Actor()
    {}

    Object3D::Object3D(Geometry* geometry, Material* material):
        Actor(geometry, material)
    {}

    Object3D::~Object3D()
    {}

}
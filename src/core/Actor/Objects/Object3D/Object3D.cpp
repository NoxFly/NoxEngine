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
        Actor<V3D>()
    {}

    Object3D::Object3D(Geometry* geometry, Material<V3D>* material):
        Actor<V3D>(geometry, material)
    {}

    Object3D::~Object3D()
    {}

}
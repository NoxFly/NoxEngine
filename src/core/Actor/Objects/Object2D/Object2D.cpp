/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Object2D.hpp"

#include "Console/Console.hpp"


namespace NoxEngine {

    Object2D::Object2D():
        Actor<V2D>()
    {}

    Object2D::Object2D(Geometry* geometry, Material<V2D>* material):
        Actor<V2D>(geometry, material)
    {}

    Object2D::~Object2D()
    {}

}
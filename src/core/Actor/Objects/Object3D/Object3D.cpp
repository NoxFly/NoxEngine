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
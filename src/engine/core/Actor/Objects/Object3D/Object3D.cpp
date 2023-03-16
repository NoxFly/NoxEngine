#include "Object3D.hpp"

#include "Console/Console.hpp"


namespace NoxEngine {

    Object3D::Object3D():
        Actor<V3D>()
    {}

    Object3D::Object3D(const Geometry& geometry, const Material& material):
        Actor<V3D>(geometry, material)
    {}

    Object3D::~Object3D()
    {}

}
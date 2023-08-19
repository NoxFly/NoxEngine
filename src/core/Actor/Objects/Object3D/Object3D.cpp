#include "Object3D.hpp"

#include "Console/Console.hpp"


namespace NoxEngine {

    Object3D::Object3D():
        Actor<V3D>()
    {}

    Object3D::Object3D(const Geometry& geometry, const Material<V3D>& material):
        Actor<V3D>(geometry, material)
    {
        std::cout << "Object3D constructor" << std::endl;
    }

    Object3D::~Object3D()
    {}

}
#include "Object3D.hpp"

#include "Console.hpp"


namespace NoxEngine {

    Object3D::Object3D():
        Actor<V3D>()
    {}

    Object3D::Object3D(const Geometry& geometry, const Material& material):
        Actor<V3D>(geometry, material)
    {
        if(!load()) {
            Console::error("Object3D", "Failed to load.");
        }
    }

    Object3D::~Object3D()
    {}

}
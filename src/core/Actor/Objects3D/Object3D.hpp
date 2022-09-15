#ifndef OBJECT_3D_HPP
#define OBJECT_3D_HPP

#include <glm/glm.hpp>

#include "engine.typedef.hpp"
#include "Actor.hpp"
#include "Geometry.hpp"
#include "Material.hpp"
#include "Matrices.hpp"


namespace NoxEngine {

    class Object3D: public Actor<V3D> {
        public:
            explicit Object3D();
            explicit Object3D(const Geometry& geometry, const Material& material);
            ~Object3D();

    };

}

#endif // OBJECT_3D_HPP
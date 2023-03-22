#ifndef OBJECT_3D_HPP
#define OBJECT_3D_HPP

#include <glm/glm.hpp>
#include <memory>

#include "core/engine.typedef.hpp"
#include "core/Actor/Actor.hpp"
#include "core/Actor/Geometries/Geometry.hpp"
#include "core/Actor/Materials/Material.hpp"
#include "core/MatricesMVP/Matrices.hpp"


namespace NoxEngine {

    class Object3D: public Actor<V3D> {
        public:
            explicit Object3D();
            explicit Object3D(const Geometry& geometry, const Material& material);
            ~Object3D();

    };

}

#endif // OBJECT_3D_HPP
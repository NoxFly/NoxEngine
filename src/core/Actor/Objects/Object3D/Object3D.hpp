/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

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

    class Object3D: public Actor {
        public:
            explicit Object3D();
            explicit Object3D(Geometry* geometry, Material* material);
            ~Object3D();
    };

}

#endif // OBJECT_3D_HPP
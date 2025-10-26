/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "engine/core/engine.typedef.hpp"
#include "engine/core/MatricesMVP/Matrices.hpp"
#include "engine/core/Scene/Scene.hpp"
#include "Component.hpp"

namespace NoxEngine {

    class Drawable: public ActorComponent {
        public:
            virtual ~Drawable() = default;

			virtual void draw(Scene* scene, Matrices& mvp, const V3D& cameraPosition) = 0;
    };

}

#endif // DRAWABLE_HPP
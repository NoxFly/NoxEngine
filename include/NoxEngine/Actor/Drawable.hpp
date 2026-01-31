/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "NoxEngine/engine.typedef.hpp"
#include "NoxEngine/Camera/Matrices.hpp"
#include "NoxEngine/Rendering/Scene.hpp"
#include "NoxEngine/Actor/Component.hpp"

namespace NoxEngine {

    class Drawable: public ActorComponent {
        public:
            virtual ~Drawable() = default;

			virtual void draw(Scene* scene, Matrices& mvp, const V3D& cameraPosition) = 0;
    };

}

#endif // DRAWABLE_HPP
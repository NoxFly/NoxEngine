#ifndef ENGINE_HPP
#define ENGINE_HPP

// preload : common to both engines

#define GLM_ENABLE_EXPERIMENTAL

#include "core/engine.typedef.hpp"

#include "core/Actor/Actor.hpp"
#include "core/Scene/Scene.hpp"






// ------------------------------ DIMENSION 2 ------------------------------
#if defined(__NOX_ENGINE_2D__)

#define _2D

namespace NoxEngine {

    typedef Scene<V2D> Scene2D;
    typedef Actor<V2D> Actor2D;

}

#include "core/Camera/2D/Camera2D/Camera2D.hpp"
#include "core/MatricesMVP/Matrices2D/Matrices2D.hpp"
#include "core/Actor/Object2D/Rectangle/Rectangle.hpp"











// ------------------------------ DIMENSION 3 ------------------------------
#elif defined(__NOX_ENGINE_3D__)

#define _3D

namespace NoxEngine {

    typedef Actor<V3D> Actor3D;
    typedef Scene<V3D> Scene3D;

}

#include "core/Camera/3D/PerspectiveCamera/PerspectiveCamera.hpp"
#include "core/MatricesMVP/Matrices3D/Matrices3D.hpp"
#include "core/Actor/Objects3D/Cube/Cube.hpp"













// -------------------------------------------------------------------------

#endif // __NOX_ENGINE_*D__







// postload : common to both engines
#include "core/Renderer/Renderer.hpp"
#include "core/Actor/Shader/Shader.hpp"
#include "core/Actor/Texture/Texture.hpp"


#endif // ENGINE_HPP
#ifndef ENGINE_HPP
#define ENGINE_HPP

// preload : common to both engines

#define GLM_ENABLE_EXPERIMENTAL

#include "engine.typedef.hpp"

#include "Actor.hpp"
#include "Scene.hpp"






// ------------------------------ DIMENSION 2 ------------------------------
#if defined(__NOX_ENGINE_2D__)

#define _2D

namespace NoxEngine {

    typedef Scene<V2D> Scene2D;
    typedef Actor<V2D> Actor2D;

}

#include "Camera2D.hpp"
#include "Matrices2D.hpp"
#include "Rectangle.hpp"











// ------------------------------ DIMENSION 3 ------------------------------
#elif defined(__NOX_ENGINE_3D__)

#define _3D

namespace NoxEngine {

    typedef Actor<V3D> Actor3D;
    typedef Scene<V3D> Scene3D;

}

#include "PerspectiveCamera.hpp"
#include "Matrices3D.hpp"
#include "Cube.hpp"













// -------------------------------------------------------------------------

#endif // __NOX_ENGINE_*D__







// postload : common to both engines
#include "Renderer.hpp"
#include "Shader.hpp"
#include "Texture.hpp"


#endif // ENGINE_HPP
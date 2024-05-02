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
#include "core/Actor/Objects/Object2D/Rectangle/Rectangle.hpp"
#include "core/Actor/Geometries/2D/RectangleGeometry/RectangleGeometry.hpp"
#include "core/Actor/Geometries/2D/CircleGeometry/CircleGeometry.hpp"
#include "core/Actor/Materials/2D/Basic2DMaterial/Basic2DMaterial.hpp"










// ------------------------------ DIMENSION 3 ------------------------------
#elif defined(__NOX_ENGINE_3D__)

#define _3D

namespace NoxEngine {

    typedef Actor<V3D> Actor3D;
    typedef Scene<V3D> Scene3D;

}

#include "core/Camera/3D/PerspectiveCamera/PerspectiveCamera.hpp"
#include "core/Camera/3D/OrthographicCamera/OrthographicCamera.hpp"
#include "core/Actor/Objects/Object3D/Mesh3D/Mesh3D.hpp"
#include "core/Actor/Objects/Object3D/Cube/Cube.hpp"
#include "core/Actor/Geometries/3D/BoxGeometry/BoxGeometry.hpp"
#include "core/Actor/Geometries/3D/SphereGeometry/SphereGeometry.hpp"
#include "core/Actor/Light/AmbientLight/AmbientLight.hpp"
#include "core/Actor/Materials/3D/Basic3DMaterial/Basic3DMaterial.hpp"
#include "core/Actor/Materials/3D/PhongMaterial/PhongMaterial.hpp"












// -------------------------------------------------------------------------

#endif // __NOX_ENGINE_*D__







// postload : common to both engines
#include "core/MatricesMVP/Matrices.hpp"
#include "core/Renderer/Renderer.hpp"
#include "core/Actor/Shader/Shader.hpp"
#include "core/Actor/Texture/Texture.hpp"

#include "core/Actor/Light/AmbientLight/AmbientLight.hpp"


#endif // ENGINE_HPP
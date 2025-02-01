/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef ENGINE_HPP
#define ENGINE_HPP

// preload

#define GLM_ENABLE_EXPERIMENTAL

#include "core/engine.typedef.hpp"

#include "core/Actor/Actor.hpp"
#include "core/Scene/Scene.hpp"



#include "core/Camera/PerspectiveCamera/PerspectiveCamera.hpp"
#include "core/Camera/OrthographicCamera/OrthographicCamera.hpp"
#include "core/Actor/Objects/Object3D/Mesh3D/Mesh3D.hpp"
#include "core/Actor/Objects/Object3D/Cube/Cube.hpp"
#include "core/Actor/Geometries/3D/BoxGeometry/BoxGeometry.hpp"
#include "core/Actor/Geometries/3D/SphereGeometry/SphereGeometry.hpp"
#include "core/Actor/Light/AmbientLight/AmbientLight.hpp"
#include "core/Actor/Materials/BasicMaterial/BasicMaterial.hpp"
#include "core/Actor/Materials/PhongMaterial/PhongMaterial.hpp"
#include "core/Controls/3D/PointerLockControls/PointerLockControls.hpp"

// -------------------------------------------------------------------------


// postload
#include "core/MatricesMVP/Matrices.hpp"
#include "core/Renderer/Renderer.hpp"
#include "core/Actor/Shader/Shader.hpp"
#include "core/Actor/Texture/Texture.hpp"

#include "core/Actor/Light/AmbientLight/AmbientLight.hpp"


#endif // ENGINE_HPP
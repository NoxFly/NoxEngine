/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef ENGINE_HPP
#define ENGINE_HPP

#define GLM_ENABLE_EXPERIMENTAL

// Utilities
#include "IniSet/IniSet.hpp"
#include "Console/Console.hpp"


// Core
#include "core/engine.typedef.hpp"

#include "core/MatricesMVP/Matrices.hpp"
#include "core/Actor/Shader/Shader.hpp"
#include "core/Actor/Texture/Texture.hpp"

#include "core/Actor/Actor.hpp"

#include "core/Renderer/Renderer.hpp"
#include "core/Scene/Scene.hpp"

// Cameras
#include "core/Camera/PerspectiveCamera/PerspectiveCamera.hpp"
#include "core/Camera/OrthographicCamera/OrthographicCamera.hpp"

// Camera Controls
#include "core/Controls/3D/PointerLockControls/PointerLockControls.hpp"
#include "core/Controls/3D/PointerLockControls/FPSControls/FPSControls.hpp"
#include "core/Controls/3D/PointerLockControls/TPSControls/TPSControls.hpp"
#include "core/Controls/3D/ArcballControls/ArcballControls.hpp"
#include "core/Controls/3D/StrategyControls/StrategyControls.hpp"

// Objects
#include "core/Actor/Objects/Object3D/Mesh3D/Mesh3D.hpp"
#include "core/Actor/Objects/Object3D/Cube/Cube.hpp"

// Geometries
#include "core/Actor/Geometries/3D/BoxGeometry/BoxGeometry.hpp"
#include "core/Actor/Geometries/3D/SphereGeometry/SphereGeometry.hpp"

// Materials
#include "core/Actor/Materials/BasicMaterial/BasicMaterial.hpp"
#include "core/Actor/Materials/PhongMaterial/PhongMaterial.hpp"

// Lights
#include "core/Actor/Light/AmbientLight/AmbientLight.hpp"


#endif // ENGINE_HPP
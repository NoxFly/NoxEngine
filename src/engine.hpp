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
#include "engine/core/engine.typedef.hpp"

#include "engine/core/MatricesMVP/Matrices.hpp"
#include "engine/Rendering/Shader/Shader.hpp"
#include "engine/Rendering/Texture/Texture.hpp"

// Actor
#include "engine/core/Actor/Actor.hpp"
#include "engine/core/Actor/Component.hpp"
#include "engine/core/Actor/Drawable.hpp"
#include "engine/core/Actor/Movable.hpp"

// Renderer & Scene
#include "engine/core/Renderer/Renderer.hpp"
#include "engine/core/Scene/Scene.hpp"

// Cameras
#include "engine/core/Camera/PerspectiveCamera/PerspectiveCamera.hpp"
#include "engine/core/Camera/OrthographicCamera/OrthographicCamera.hpp"

// Camera Controls
#include "engine/core/Controls/3D/PointerLockControls/PointerLockControls.hpp"
#include "engine/core/Controls/3D/PointerLockControls/FPSControls/FPSControls.hpp"
#include "engine/core/Controls/3D/PointerLockControls/TPSControls/TPSControls.hpp"
#include "engine/core/Controls/3D/ArcballControls/ArcballControls.hpp"
#include "engine/core/Controls/3D/StrategyControls/StrategyControls.hpp"

// Drawable Objects
#include "engine/Rendering/Components/Mesh/Mesh.hpp"

// Geometries
#include "engine/Rendering/Geometries/3D/BoxGeometry/BoxGeometry.hpp"
#include "engine/Rendering/Geometries/3D/SphereGeometry/SphereGeometry.hpp"

// Materials
#include "engine/Rendering/Materials/BasicMaterial/BasicMaterial.hpp"
#include "engine/Rendering/Materials/PhongMaterial/PhongMaterial.hpp"

// Lights
#include "engine/Rendering/Light/AmbientLight/AmbientLight.hpp"


// Loaders
#include "engine/Loaders/OBJLoader/OBJLoader.hpp"
#include "engine/Loaders/MTLLoader/MTLLoader.hpp"


#endif // ENGINE_HPP
/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef ENGINE_HPP
#define ENGINE_HPP

#define GLM_ENABLE_EXPERIMENTAL

// Utilities
#include "NoxEngine/utils/IniSet.hpp"
#include "NoxEngine/utils/Console.hpp"


// Core
#include "NoxEngine/engine.typedef.hpp"

#include "NoxEngine/Camera/Matrices.hpp"
#include "NoxEngine/Rendering/Shader.hpp"
#include "NoxEngine/Rendering/Texture.hpp"

// Actor
#include "NoxEngine/Actor/Actor.hpp"
#include "NoxEngine/Actor/Component.hpp"
#include "NoxEngine/Actor/Drawable.hpp"
#include "NoxEngine/Actor/Movable.hpp"

// Renderer & Scene
#include "NoxEngine/Rendering/Renderer.hpp"
#include "NoxEngine/Rendering/Scene.hpp"

// Cameras
#include "NoxEngine/Camera/PerspectiveCamera.hpp"
#include "NoxEngine/Camera/OrthographicCamera.hpp"

// Camera Controls
#include "NoxEngine/Controls/PointerLockControls.hpp"
#include "NoxEngine/Controls/FPSControls/FPSControls.hpp"
#include "NoxEngine/Controls/TPSControls/TPSControls.hpp"
#include "NoxEngine/Controls/ArcballControls.hpp"
#include "NoxEngine/Controls/StrategyControls.hpp"

// Drawable Objects
#include "NoxEngine/Mesh/Mesh.hpp"

// Geometries
#include "NoxEngine/Geometry/BoxGeometry.hpp"
#include "NoxEngine/Geometry/SphereGeometry.hpp"

// Materials
#include "NoxEngine/Material/UnlitMaterial.hpp"
#include "NoxEngine/Material/PhongMaterial.hpp"
#include "NoxEngine/Material/LambertMaterial.hpp"
#include "NoxEngine/Material/PBRMaterial.hpp"

// Lights
#include "NoxEngine/Light/AmbientLight.hpp"
#include "NoxEngine/Light/SpotLight.hpp"
#include "NoxEngine/Light/DirectionalLight.hpp"
#include "NoxEngine/Light/RectAreaLight.hpp"


// Loaders
#include "NoxEngine/Loader/OBJLoader.hpp"
#include "NoxEngine/Loader/MTLLoader.hpp"
#include "NoxEngine/Loader/TextureLoader.hpp"
#include "NoxEngine/Loader/ShaderLoader.hpp"


#endif // ENGINE_HPP
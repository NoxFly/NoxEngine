// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

// Animation
#include <NoxEngine/animation/AnimationClip.hpp>
#include <NoxEngine/animation/AnimationPlayer.hpp>
#include <NoxEngine/animation/Skeleton.hpp>

// Audio
#include <NoxEngine/audio/AudioSystem.hpp>

// Editor
#include <NoxEngine/editor/AssetBrowser.hpp>
#include <NoxEngine/editor/Gizmo.hpp>
#include <NoxEngine/editor/PropertyInspector.hpp>
#include <NoxEngine/editor/SceneHierarchy.hpp>
#include <NoxEngine/editor/SceneSerializer.hpp>

// Core
#include <NoxEngine/core/AssetCache.hpp>
#include <NoxEngine/core/Assert.hpp>
#include <NoxEngine/core/Color.hpp>
#include <NoxEngine/core/Engine.hpp>
#include <NoxEngine/core/FileWatcher.hpp>
#include <NoxEngine/core/Logger.hpp>
#include <NoxEngine/core/Serialization.hpp>
#include <NoxEngine/core/Signal.hpp>

// Math
#include <NoxEngine/math/Types.hpp>

// Network
#include <NoxEngine/network/EntityReplication.hpp>
#include <NoxEngine/network/NetworkSession.hpp>
#include <NoxEngine/network/NetworkSystem.hpp>

// Renderer
#include <NoxEngine/renderer/Billboard.hpp>
#include <NoxEngine/renderer/Camera.hpp>
#include <NoxEngine/renderer/PerspectiveCamera.hpp>
#include <NoxEngine/renderer/OrthographicCamera.hpp>
#include <NoxEngine/renderer/CameraController.hpp>
#include <NoxEngine/renderer/CascadedShadowMap.hpp>
#include <NoxEngine/renderer/ClusteredLighting.hpp>
#include <NoxEngine/renderer/ComputeShader.hpp>
#include <NoxEngine/renderer/CustomRenderPass.hpp>
#include <NoxEngine/renderer/Decal.hpp>
#include <NoxEngine/renderer/Frustum.hpp>
#include <NoxEngine/renderer/GBuffer.hpp>
#include <NoxEngine/renderer/GPUInstancing.hpp>
#include <NoxEngine/renderer/GPUParticleSystem.hpp>
#include <NoxEngine/renderer/IndirectDraw.hpp>
#include <NoxEngine/renderer/PostProcessEffect.hpp>
#include <NoxEngine/renderer/ImposterSystem.hpp>
#include <NoxEngine/renderer/InstancedVegetation.hpp>
#include <NoxEngine/renderer/OcclusionCulling.hpp>
#include <NoxEngine/renderer/PostProcessStack.hpp>
#include <NoxEngine/renderer/RenderContext.hpp>
#include <NoxEngine/renderer/ShaderPreprocessor.hpp>
#include <NoxEngine/renderer/Sky.hpp>
#include <NoxEngine/renderer/TextureLoader.hpp>
#include <NoxEngine/renderer/effects/BloomEffect.hpp>
#include <NoxEngine/renderer/effects/ChromaticAberrationEffect.hpp>
#include <NoxEngine/renderer/effects/FXAAEffect.hpp>
#include <NoxEngine/renderer/effects/FilmGrainEffect.hpp>
#include <NoxEngine/renderer/effects/SSAOEffect.hpp>
#include <NoxEngine/renderer/effects/SSREffect.hpp>
#include <NoxEngine/renderer/effects/TAAEffect.hpp>
#include <NoxEngine/renderer/effects/VignetteEffect.hpp>
#include <NoxEngine/renderer/effects/VolumetricFogEffect.hpp>

// Physics
#include <NoxEngine/physics/CharacterController.hpp>
#include <NoxEngine/physics/PhysicsDebugRenderer.hpp>
#include <NoxEngine/physics/PhysicsWorld.hpp>
#include <NoxEngine/physics/RigidBody.hpp>

// Scene
#include <NoxEngine/scene/Geometry.hpp>
#include <NoxEngine/scene/Light.hpp>
#include <NoxEngine/scene/Material.hpp>
#include <NoxEngine/scene/Mesh.hpp>
#include <NoxEngine/scene/ModelLoader.hpp>
#include <NoxEngine/scene/ObjectPicker.hpp>
#include <NoxEngine/scene/Scene3D.hpp>
#include <NoxEngine/scene/SceneNode.hpp>
#include <NoxEngine/scene/SceneObject.hpp>
#include <NoxEngine/scene/Terrain.hpp>

// Scripting
#include <NoxEngine/scripting/LuaScriptEngine.hpp>
#include <NoxEngine/scripting/PluginSystem.hpp>
#include <NoxEngine/scene/Transform.hpp>

// Platform
#include <NoxEngine/platform/Input.hpp>
#include <NoxEngine/platform/InputMapping.hpp>
#include <NoxEngine/platform/Window.hpp>

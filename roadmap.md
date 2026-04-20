## Roadmap

### v0.1 — Foundation  ✅ done
- [x] CMake project structure (engine lib + sandbox exe)
- [x] SDL3 window + OpenGL 4.6 context
- [x] Core systems: Logger, Assert, HandlePool, Signal
- [x] Math types (GLM wrappers + AABB)
- [x] OpenGL RHI — buffers, textures, shaders (DSA)
- [x] Transform + SceneNode hierarchy
- [x] Friendly high-level API: Engine, Scene3D, PerspectiveCamera
- [x] Mesh + Geometry factory (box, sphere, plane)
- [x] Material::standard() — Blinn-Phong shading
- [x] DirectionalLight, PointLight, AmbientLight
- [x] Main loop via engine.run(fn)
- [x] Ray math type

### v0.2 — Usable renderer  ✅ done
- [x] Frustum culling (AABB per mesh)
- [x] Render queue sorting (opaque front-to-back, transparent back-to-front)
- [x] Multiple lights (up to 8 point lights + 4 directional)
- [x] Texture loading via stb_image (albedo map)
- [x] glTF 2.0 model import via fastgltf (engine.load)
- [x] Geometry::cylinder, Geometry::fromVertices
- [x] Material::unlit()
- [x] OrthographicCamera
- [x] FPS counter + debug overlay (Dear ImGui)

### v0.3 — Quality of life  ✅ done
- [x] Scene graph traversal utilities (find by name, iterate by type)
- [x] Asset cache (no double-load for same path)
- [x] Hot-reload shaders in Debug builds
- [x] CameraController helpers (orbit, fly, first-person)
- [x] Input system (keyboard, mouse, gamepad via SDL3)
- [x] Window resize handling (framebuffer + camera aspect)

### v0.4 — PBR & lighting  ✅ done
- [x] PBR Material (metallic/roughness workflow)
- [ ] Image-based lighting (IBL) — environment maps, irradiance, prefiltered
- [x] HDR rendering + tone mapping (exposure-based)
- [x] Directional light shadow maps (PCF 3x3)
- [x] Point light shadow maps (cube maps, 6-pass)
- [x] Normal mapping
- [x] Emissive maps

### v0.5 — Post-processing  ✅ done
- [x] Framebuffer / render-to-texture infrastructure
- [x] FXAA anti-aliasing
- [x] Bloom
- [x] SSAO (Screen Space Ambient Occlusion)
- [x] Gamma correction + exposure control
- [x] Post-process stack (ordered chain of effects)

### v0.6 — Animation  ✅ done
- [x] Skeletal animation (skin/joints from glTF)
- [x] Animation clips + playback (play, pause, loop, speed)
- [x] Animation blending (cross-fade between clips)
- [x] Morph targets (blend shapes)

### v0.7 — Physics integration  ✅ done
- [x] Jolt Physics integration (rigid bodies, collision shapes)
- [x] RigidBody component (static, dynamic, kinematic)
- [x] Collider shapes (box, sphere, capsule, mesh)
- [x] Ray casting API (scene.raycast(origin, direction))
- [x] Collision event callbacks (Signal-based)

### v0.8 — Vulkan backend
- [ ] VulkanRHI implementing the RHI interface
- [ ] Vulkan Memory Allocator (VMA) integration
- [ ] Backend selection at startup (--backend vulkan / opengl)
- [ ] Feature parity with OpenGL backend
- [ ] Validation layers in Debug builds

### v0.9 — Editor tooling
- [ ] Dear ImGui scene hierarchy panel
- [ ] Property inspector (transform, material, light params)
- [ ] Asset browser
- [ ] Gizmos (translate, rotate, scale handles)
- [ ] Scene save/load (JSON or custom binary format)

### v1.0 — Stable public release
- [ ] API stability guarantee — semver from here
- [ ] Full Doxygen documentation
- [ ] Tutorial series (01-window, 02-mesh, 03-lighting, 04-model-import...)
- [ ] CMake install target + find_package(NoxEngine) support
- [ ] CI/CD on GitHub Actions (Windows MSVC, Ubuntu GCC, Ubuntu Clang)
- [ ] vcpkg port

### v1.1 — Advanced rendering
- [ ] Deferred rendering pipeline (G-Buffer : position, normal, albedo, material)
- [ ] Tiled / Clustered lighting (hundreds of lights perf loss)
- [ ] Cascaded Shadow Maps (CSM) for large exterior scenes
- [ ] Screen Space Reflections (SSR)
- [ ] Temporal Anti-Aliasing (TAA)
- [ ] Volumetric fog / god rays
- [ ] Decals (bullet impacts, stains, splashes on surfaces)

### v1.2 — Terrain & large worlds
- [ ] Heightmap terrain with LOD (geomipmapping)
- [ ] Terrain splatting (blend of many textures depending the pente/altitude)
- [ ] Streaming of terrain's chunks (loading/unloading on the fly around the player)
- [ ] Instantiated vegetation (grass, trees) via GPU instancing
- [ ] Imposters for the remote objects (automatic billboard)
- [ ] Occlusion culling (GPU occlusion queries or HZB)

### v1.3 — GPU & advanced performance
- [ ] GPU instancing API high level (scene.addInstanced(mesh, transforms))
- [ ] Indirect draw calls (GL_DRAW_INDIRECT_BUFFER / VkDrawIndirectCommand)
- [ ] Compute shaders — exposed API to engine's level
- [ ] GPU particle system (compute-driven, millions of particles)
- [ ] Mesh shaders (Vulkan / DX12 backend required)
- [ ] Bindless textures (Vulkan)

### v1.4 — Spatial audio
- [ ] Integration of miniaudio (header-only)
- [ ] 3D positionnal Audio (distance attenuation, Doppler effect)
- [ ] Reverb zones (interior/exterior environnement)
- [ ] Audio streaming for long musics
- [ ] API : AudioSource component, AudioListener on the camera

### v1.5 — Scripting & extensibility
- [ ] Plugin system (.dll/.so dynamically loaded)
- [ ] Bindings Lua via sol2 (lightweight scripting for game's logic)
- [ ] Hot-reload for the scripts on Debug mode
- [ ] Generic serialization API (static reflection via macros or concepts)
- [ ] Custom render passes recordable from the application

### v1.6 — Networking foundation
- [ ] Low level network abstraction (UDP via ENet or GameNetworkingSockets)
- [ ] Snapshot interpolation + client reconciliation
- [ ] basic Entity replication (position, rotation synchronized)
- [ ] Lobby / session management API
- [ ] Deterministic simulation (Guaranteed fixed timestep + shared seed)

### v1.7 — DirectX 12 backend
- [ ] DX12RHI implementing the RHI interface
- [ ] DirectStorage for ultra-fast asset loading (Windows only)
- [ ] PIX integration for GPU profiling on Windows
- [ ] Feature parity Vulkan/DX12/OpenGL

### v2.0 — Complete Engine production-ready
- [ ] Complete world streaming (open world without visible loading time)
- [ ] Nanite-like virtualized geometry (Automatic LOD at the triangle)
- [ ] Lumen-like global dynamic illumination (radiance cache)
- [ ] Full Vulkan/DX12 by default, OpenGL as fallback legacy
- [ ] Editor standalone (separated application built on NoxEngine)
- [ ] Marketplace of plugins and assets
- [ ] Exhaustive documentation + examples for each system

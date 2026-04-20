## Roadmap

### v0.1 — Foundation  🔨 in progress
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
- [ ] Ray math type

### v0.2 — Usable renderer
- [ ] Frustum culling (AABB per mesh)
- [ ] Render queue sorting (opaque front-to-back, transparent back-to-front)
- [ ] Multiple lights (up to 8 point lights + 1 directional)
- [ ] Texture loading via stb_image (albedo, normal maps)
- [ ] glTF 2.0 model import via fastgltf (engine.load)
- [ ] Geometry::cylinder, Geometry::fromVertices
- [ ] Material::unlit()
- [ ] OrthographicCamera
- [ ] FPS counter + debug overlay (Dear ImGui)

### v0.3 — Quality of life
- [ ] Scene graph traversal utilities (find by name, iterate by type)
- [ ] Asset cache (no double-load for same path)
- [ ] Hot-reload shaders in Debug builds
- [ ] CameraController helpers (orbit, fly, first-person)
- [ ] Input system (keyboard, mouse, gamepad via SDL3)
- [ ] Window resize handling (framebuffer + camera aspect)

### v0.4 — PBR & lighting
- [ ] PBR Material (metallic/roughness workflow)
- [ ] Image-based lighting (IBL) — environment maps, irradiance, prefiltered
- [ ] HDR rendering + tone mapping (ACES / Reinhard)
- [ ] Directional light shadow maps (PCF)
- [ ] Point light shadow maps (cube maps)
- [ ] Normal mapping
- [ ] Emissive maps

### v0.5 — Post-processing
- [ ] Framebuffer / render-to-texture infrastructure
- [ ] FXAA anti-aliasing
- [ ] Bloom
- [ ] SSAO (Screen Space Ambient Occlusion)
- [ ] Gamma correction + exposure control
- [ ] Post-process stack (ordered chain of effects)

### v0.6 — Animation
- [ ] Skeletal animation (skin/joints from glTF)
- [ ] Animation clips + playback (play, pause, loop, speed)
- [ ] Animation blending (cross-fade between clips)
- [ ] Morph targets (blend shapes)

### v0.7 — Physics integration
- [ ] Jolt Physics integration (rigid bodies, collision shapes)
- [ ] RigidBody component (static, dynamic, kinematic)
- [ ] Collider shapes (box, sphere, capsule, mesh)
- [ ] Ray casting API (scene.raycast(origin, direction))
- [ ] Collision event callbacks (Signal-based)

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

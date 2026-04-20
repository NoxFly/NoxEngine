// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/core/Engine.hpp>
#include <NoxEngine/core/Logger.hpp>
#include <NoxEngine/renderer/Frustum.hpp>
#include <NoxEngine/renderer/Renderer.hpp>
#include <NoxEngine/renderer/RHI.hpp>
#include <NoxEngine/renderer/TextureLoader.hpp>
#include <NoxEngine/scene/Geometry.hpp>
#include <NoxEngine/scene/Light.hpp>
#include <NoxEngine/scene/Mesh.hpp>
#include <NoxEngine/scene/ModelLoader.hpp>

#include <GL/glew.h>
#include <SDL3/SDL.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace Nox {

    namespace {
        [[nodiscard]] std::string readFileContents(const std::filesystem::path& path) {
            std::ifstream file(path, std::ios::in);
            if (!file.is_open()) {
                return {};
            }
            std::ostringstream ss;
            ss << file.rdbuf();
            return ss.str();
        }
    }

    // ── Embedded shaders ───────────────────────────────────────────

    static constexpr int MaxDirectionalLights = 4;
    static constexpr int MaxPointLights = 8;

    static constexpr std::string_view VertexShaderSource = R"glsl(
    #version 460 core

    layout(location = 0) in vec3 aPosition;
    layout(location = 1) in vec3 aNormal;
    layout(location = 2) in vec2 aUV;

    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;

    out vec3 vWorldPos;
    out vec3 vNormal;
    out vec2 vUV;

    void main() {
        vec4 worldPos = uModel * vec4(aPosition, 1.0);
        vWorldPos = worldPos.xyz;
        vNormal   = mat3(transpose(inverse(uModel))) * aNormal;
        vUV       = aUV;
        gl_Position = uProjection * uView * worldPos;
    }
    )glsl";

    static constexpr std::string_view FragmentShaderSource = R"glsl(
    #version 460 core

    in vec3 vWorldPos;
    in vec3 vNormal;
    in vec2 vUV;

    struct DirLight {
        vec3  direction;
        vec3  color;
        float intensity;
    };

    struct PointLightData {
        vec3  position;
        vec3  color;
        float intensity;
        float range;
    };

    uniform int uNumDirLights;
    uniform DirLight uDirLights[4];

    uniform int uNumPointLights;
    uniform PointLightData uPointLights[8];

    uniform vec3  uAmbientColor;
    uniform float uAmbientIntensity;
    uniform vec3  uCameraPos;
    uniform vec3  uObjectColor;

    uniform bool uHasAlbedoTex;
    uniform sampler2D uAlbedoTex;

    out vec4 FragColor;

    void main() {
        vec3 N = normalize(vNormal);
        vec3 V = normalize(uCameraPos - vWorldPos);

        vec3 baseColor = uObjectColor;
        if (uHasAlbedoTex) {
            baseColor *= texture(uAlbedoTex, vUV).rgb;
        }

        // Ambient
        vec3 result = uAmbientColor * uAmbientIntensity;

        // Directional lights
        for (int i = 0; i < uNumDirLights; ++i) {
            vec3 L = normalize(-uDirLights[i].direction);
            float diff = max(dot(N, L), 0.0);
            vec3 H = normalize(L + V);
            float spec = pow(max(dot(N, H), 0.0), 32.0);
            result += uDirLights[i].color * uDirLights[i].intensity * (diff + spec * 0.5);
        }

        // Point lights
        for (int i = 0; i < uNumPointLights; ++i) {
            vec3 toLight = uPointLights[i].position - vWorldPos;
            float dist = length(toLight);
            vec3 L = toLight / max(dist, 0.001);

            float attenuation = 1.0 / (1.0 + dist * dist / max(uPointLights[i].range * uPointLights[i].range, 0.001));

            float diff = max(dot(N, L), 0.0);
            vec3 H = normalize(L + V);
            float spec = pow(max(dot(N, H), 0.0), 32.0);

            result += uPointLights[i].color * uPointLights[i].intensity * attenuation * (diff + spec * 0.5);
        }

        FragColor = vec4(result * baseColor, 1.0);
    }
    )glsl";

    // ── Unlit shaders ──────────────────────────────────────────────

    static constexpr std::string_view UnlitVertexShaderSource = R"glsl(
    #version 460 core

    layout(location = 0) in vec3 aPosition;
    layout(location = 1) in vec3 aNormal;
    layout(location = 2) in vec2 aUV;

    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;

    out vec2 vUV;

    void main() {
        vUV = aUV;
        gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
    }
    )glsl";

    static constexpr std::string_view UnlitFragmentShaderSource = R"glsl(
    #version 460 core

    in vec2 vUV;

    uniform vec3 uObjectColor;
    uniform bool uHasAlbedoTex;
    uniform sampler2D uAlbedoTex;

    out vec4 FragColor;

    void main() {
        vec3 color = uObjectColor;
        if (uHasAlbedoTex) {
            color *= texture(uAlbedoTex, vUV).rgb;
        }
        FragColor = vec4(color, 1.0);
    }
    )glsl";

    // ── Engine ─────────────────────────────────────────────────────

    Engine::Engine(const EngineConfig& config) {
        window_ = std::make_unique<Window>(config.title, config.width, config.height);

        auto rhi = createOpenGLRHI();
        renderer_ = std::make_unique<Renderer>(std::move(rhi));
        renderer_->onResize(config.width, config.height);

        // Handle resize
        window_->onResize.connect([this](int w, int h) {
            renderer_->onResize(w, h);
            glViewport(0, 0, w, h);
        });

        if (!config.vsync) {
            SDL_GL_SetSwapInterval(0);
        }

        // Create lit pipeline (Blinn-Phong multi-light)
        auto& rhi_ref = renderer_->rhi();
        {
            auto vs = rhi_ref.createShader({ .stage = ShaderStage::Vertex,   .source = VertexShaderSource });
            auto fs = rhi_ref.createShader({ .stage = ShaderStage::Fragment, .source = FragmentShaderSource });
            auto pipeline = rhi_ref.createPipeline({
                .vertexShader   = vs,
                .fragmentShader = fs,
                .depthTest      = true,
                .depthWrite     = true,
                .blending       = false
            });
            litPipeline_ = pipeline.index;
        }

        // Create unlit pipeline
        {
            auto vs = rhi_ref.createShader({ .stage = ShaderStage::Vertex,   .source = UnlitVertexShaderSource });
            auto fs = rhi_ref.createShader({ .stage = ShaderStage::Fragment, .source = UnlitFragmentShaderSource });
            auto pipeline = rhi_ref.createPipeline({
                .vertexShader   = vs,
                .fragmentShader = fs,
                .depthTest      = true,
                .depthWrite     = true,
                .blending       = false
            });
            unlitPipeline_ = pipeline.index;
        }

        pipelineReady_ = true;

        debugOverlay_.init(*this);

        NOX_LOG_INFO("Engine initialized ({}x{})", config.width, config.height);
    }

    Engine::~Engine() {
        debugOverlay_.shutdown();
        renderer_.reset();
        window_.reset();
    }

    float Engine::aspect() const {
        return window_->aspect();
    }

    void Engine::run(std::function<void(float dt)> loopFn) {
        uint64_t lastTicks = SDL_GetPerformanceCounter();
        uint64_t freq = SDL_GetPerformanceFrequency();

        float fpsTimer = 0.0f;
        int   frameCount = 0;

        while (window_->pollEvents()) {
            uint64_t now = SDL_GetPerformanceCounter();
            float dt = static_cast<float>(now - lastTicks) / static_cast<float>(freq);
            lastTicks = now;

            // Clamp large dt (e.g. after breakpoint)
            if (dt > 0.1f) dt = 0.016f;

            currentFrameTime_ = dt;

#ifndef NDEBUG
            shaderWatcher_.poll();
#endif

            debugOverlay_.beginFrame();

            loopFn(dt);

            debugOverlay_.render(*this);
            debugOverlay_.endFrame();

            window_->swapBuffers();

            // FPS tracking
            ++frameCount;
            fpsTimer += dt;
            if (fpsTimer >= 1.0f) {
                currentFps_ = static_cast<float>(frameCount) / fpsTimer;
                NOX_LOG_INFO("FPS: {}", frameCount);
                frameCount = 0;
                fpsTimer -= 1.0f;
            }
        }
    }

    void Engine::render(Scene3D& scene, PerspectiveCamera& camera) {
        camera.setAspect(window_->aspect());
        renderInternal(scene, camera.viewMatrix(), camera.projectionMatrix(), camera.position());
    }

    void Engine::render(Scene3D& scene, OrthographicCamera& camera) {
        renderInternal(scene, camera.viewMatrix(), camera.projectionMatrix(), camera.position());
    }

    std::shared_ptr<SceneNode> Engine::load(const std::filesystem::path& path) {
        return ModelLoader::load(path);
    }

    void Engine::renderInternal(Scene3D& scene, const Math::Mat4& viewMatrix,
                                const Math::Mat4& projMatrix, const Math::Vec3& cameraPos) {
        auto& rhi_ref = renderer_->rhi();

        // Upload meshes that aren't on the GPU yet
        for (const auto& mesh : scene.meshes()) {
            if (!mesh->gpuReady()) {
                uploadMesh(*mesh);
            }
            // Upload texture if material has one and it's not loaded yet
            if (!mesh->material()->albedoMapPath().empty() && !mesh->material()->hasAlbedoTexture()) {
                uploadTexture(*mesh);
            }
        }

        // Frustum culling
        Frustum frustum;
        Math::Mat4 viewProj = projMatrix * viewMatrix;
        frustum.extractFromMatrix(viewProj);

        // Collect visible meshes with depth for sorting
        struct SortableMesh {
            Mesh* mesh;
            float depth;
        };
        std::vector<SortableMesh> visibleMeshes;
        visibleMeshes.reserve(scene.meshes().size());

        for (const auto& mesh : scene.meshes()) {
            mesh->updateWorldMatrix();

            // Transform AABB to world space (approximate with world matrix)
            const auto& aabb = mesh->geometry()->boundingBox();
            Math::Vec3 worldCenter = Math::Vec3(mesh->worldMatrix() * Math::Vec4(
                (aabb.min + aabb.max) * 0.5f, 1.0f));
            Math::Vec3 halfExtent = (aabb.max - aabb.min) * 0.5f;

            // Expand AABB for scale (approximate)
            Math::Vec3 scale{
                glm::length(Math::Vec3(mesh->worldMatrix()[0])),
                glm::length(Math::Vec3(mesh->worldMatrix()[1])),
                glm::length(Math::Vec3(mesh->worldMatrix()[2]))
            };
            halfExtent *= scale;

            Math::AABB worldAABB{
                worldCenter - halfExtent,
                worldCenter + halfExtent
            };

            if (!frustum.containsAABB(worldAABB)) {
                continue;
            }

            // Compute depth for sorting (distance from camera)
            float depth = glm::length(worldCenter - cameraPos);
            visibleMeshes.push_back({ mesh.get(), depth });
        }

        // Sort: opaque front-to-back (minimize overdraw)
        std::sort(visibleMeshes.begin(), visibleMeshes.end(),
            [](const SortableMesh& a, const SortableMesh& b) {
                return a.depth < b.depth;
            });

        // Gather lights
        std::vector<const DirectionalLight*> dirLights;
        std::vector<const PointLight*> pointLights;
        Math::Vec3 ambientColor{ 1.0f, 1.0f, 1.0f };
        float      ambientIntensity = 0.05f;

        for (const auto& light : scene.lights()) {
            if (auto* dir = dynamic_cast<DirectionalLight*>(light.get())) {
                if (static_cast<int>(dirLights.size()) < MaxDirectionalLights)
                    dirLights.push_back(dir);
            }
            else if (auto* pt = dynamic_cast<PointLight*>(light.get())) {
                if (static_cast<int>(pointLights.size()) < MaxPointLights)
                    pointLights.push_back(pt);
            }
            else if (auto* amb = dynamic_cast<AmbientLight*>(light.get())) {
                ambientColor = { amb->color().r, amb->color().g, amb->color().b };
                ambientIntensity = amb->intensity();
            }
        }

        // Begin rendering
        auto* cmd = rhi_ref.beginFrame();
        auto [w, h] = window_->size();
        cmd->setViewport(0, 0, w, h);
        cmd->clear(0.1f, 0.1f, 0.12f, 1.0f, 1.0f);

        // Draw each visible mesh
        for (const auto& entry : visibleMeshes) {
            auto* mesh = entry.mesh;

            // Choose pipeline based on material
            uint32_t pipeIdx = mesh->material()->isLit() ? litPipeline_ : unlitPipeline_;
            PipelineHandle pipeHandle{ pipeIdx, 1 };
            cmd->bindPipeline(pipeHandle);

            // Camera matrices
            cmd->pushConstant("uView", viewMatrix);
            cmd->pushConstant("uProjection", projMatrix);

            if (mesh->material()->isLit()) {
                // Set light uniforms
                cmd->pushConstant("uNumDirLights", static_cast<int>(dirLights.size()));
                for (int i = 0; i < static_cast<int>(dirLights.size()); ++i) {
                    std::string prefix = "uDirLights[" + std::to_string(i) + "].";
                    cmd->pushConstant(prefix + "direction", dirLights[static_cast<size_t>(i)]->direction());
                    const auto& dc = dirLights[static_cast<size_t>(i)]->color();
                    cmd->pushConstant(prefix + "color", Math::Vec3(dc.r, dc.g, dc.b));
                    cmd->pushConstant(prefix + "intensity", dirLights[static_cast<size_t>(i)]->intensity());
                }

                cmd->pushConstant("uNumPointLights", static_cast<int>(pointLights.size()));
                for (int i = 0; i < static_cast<int>(pointLights.size()); ++i) {
                    std::string prefix = "uPointLights[" + std::to_string(i) + "].";
                    cmd->pushConstant(prefix + "position", pointLights[static_cast<size_t>(i)]->transform().position());
                    const auto& pc = pointLights[static_cast<size_t>(i)]->color();
                    cmd->pushConstant(prefix + "color", Math::Vec3(pc.r, pc.g, pc.b));
                    cmd->pushConstant(prefix + "intensity", pointLights[static_cast<size_t>(i)]->intensity());
                    cmd->pushConstant(prefix + "range", pointLights[static_cast<size_t>(i)]->range());
                }

                cmd->pushConstant("uAmbientColor", ambientColor);
                cmd->pushConstant("uAmbientIntensity", ambientIntensity);
                cmd->pushConstant("uCameraPos", cameraPos);
            }

            cmd->pushConstant("uModel", mesh->worldMatrix());

            // Object color from material
            const auto& col = mesh->material()->color();
            cmd->pushConstant("uObjectColor", Math::Vec3(col.r, col.g, col.b));

            // Texture binding
            if (mesh->material()->hasAlbedoTexture()) {
                glBindTextureUnit(0, mesh->material()->albedoTextureId());
                // Set uHasAlbedoTex = true (via 1.0f since we push as float through the uniform)
                cmd->pushConstant("uHasAlbedoTex", 1);
            }
            else {
                cmd->pushConstant("uHasAlbedoTex", 0);
            }

            // Bind mesh GPU resources directly
            const auto& gpu = mesh->gpuData();
            glBindVertexArray(gpu.vao);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(gpu.indexCount),
                        GL_UNSIGNED_INT, nullptr);
        }

        rhi_ref.endFrame(cmd);
    }

    void Engine::uploadMesh(Mesh& mesh) {
        const auto& verts = mesh.geometry()->vertices();
        const auto& idxs  = mesh.geometry()->indices();

        GLuint vao = 0, vbo = 0, ibo = 0;
        glCreateVertexArrays(1, &vao);
        glCreateBuffers(1, &vbo);
        glCreateBuffers(1, &ibo);

        // Upload vertex data
        glNamedBufferStorage(vbo,
            static_cast<GLsizeiptr>(verts.size() * sizeof(Vertex)),
            verts.data(), 0);

        // Upload index data
        glNamedBufferStorage(ibo,
            static_cast<GLsizeiptr>(idxs.size() * sizeof(uint32_t)),
            idxs.data(), 0);

        // Set up VAO attributes
        glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
        glVertexArrayElementBuffer(vao, ibo);

        // Position (location 0)
        glEnableVertexArrayAttrib(vao, 0);
        glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
        glVertexArrayAttribBinding(vao, 0, 0);

        // Normal (location 1)
        glEnableVertexArrayAttrib(vao, 1);
        glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
        glVertexArrayAttribBinding(vao, 1, 0);

        // UV (location 2)
        glEnableVertexArrayAttrib(vao, 2);
        glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
        glVertexArrayAttribBinding(vao, 2, 0);

        mesh.gpuData() = Mesh::GpuData{
            .vao        = vao,
            .vbo        = vbo,
            .ibo        = ibo,
            .indexCount = static_cast<uint32_t>(idxs.size())
        };
        mesh.markGpuReady(true);
    }

    void Engine::uploadTexture(Mesh& mesh) {
        const auto& texPath = mesh.material()->albedoMapPath();

        // Check cache first
        if (textureCache_.contains(texPath)) {
            mesh.material()->setAlbedoTextureId(textureCache_.get(texPath));
            return;
        }

        auto texData = TextureLoader::load(texPath);
        if (!texData.pixels) return;

        GLuint texId = 0;
        glCreateTextures(GL_TEXTURE_2D, 1, &texId);
        glTextureStorage2D(texId, 1, GL_RGBA8, texData.width, texData.height);
        glTextureSubImage2D(texId, 0, 0, 0, texData.width, texData.height,
                            GL_RGBA, GL_UNSIGNED_BYTE, texData.pixels);

        glTextureParameteri(texId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(texId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(texId, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(texId, GL_TEXTURE_WRAP_T, GL_REPEAT);

        textureCache_.store(texPath, texId);
        mesh.material()->setAlbedoTextureId(texId);
        TextureLoader::free(texData);
    }

    void Engine::setShaderDirectory(const std::filesystem::path& dir) {
        shaderDir_ = dir;

#ifndef NDEBUG
        // Watch all .vert and .frag files in the directory
        if (std::filesystem::exists(dir)) {
            for (const auto& entry : std::filesystem::directory_iterator(dir)) {
                auto ext = entry.path().extension().string();
                if (ext == ".vert" || ext == ".frag" || ext == ".glsl") {
                    shaderWatcher_.watch(entry.path());
                }
            }

            shaderWatcher_.onFileChanged.connect([this]([[maybe_unused]] const std::filesystem::path& path) {
                NOX_LOG_INFO("Shader file changed: {}", path.string());
                rebuildPipelines();
            });
        }
#endif
    }

    void Engine::rebuildPipelines() {
        if (shaderDir_.empty()) {
            return;
        }

        auto& rhi_ref = renderer_->rhi();

        // Try loading external shader files, fall back to embedded
        auto litVsPath   = shaderDir_ / "lit.vert";
        auto litFsPath   = shaderDir_ / "lit.frag";
        auto unlitVsPath = shaderDir_ / "unlit.vert";
        auto unlitFsPath = shaderDir_ / "unlit.frag";

        // Rebuild lit pipeline
        {
            std::string vsSource = readFileContents(litVsPath);
            std::string fsSource = readFileContents(litFsPath);

            if (vsSource.empty()) { vsSource = std::string(VertexShaderSource); }
            if (fsSource.empty()) { fsSource = std::string(FragmentShaderSource); }

            auto vs = rhi_ref.createShader({ .stage = ShaderStage::Vertex,   .source = vsSource });
            auto fs = rhi_ref.createShader({ .stage = ShaderStage::Fragment, .source = fsSource });
            auto pipeline = rhi_ref.createPipeline({
                .vertexShader   = vs,
                .fragmentShader = fs,
                .depthTest      = true,
                .depthWrite     = true,
                .blending       = false
            });

            if (pipeline.index != 0) {
                rhi_ref.destroyPipeline({ litPipeline_ });
                litPipeline_ = pipeline.index;
                NOX_LOG_INFO("Lit pipeline reloaded");
            }
        }

        // Rebuild unlit pipeline
        {
            std::string vsSource = readFileContents(unlitVsPath);
            std::string fsSource = readFileContents(unlitFsPath);

            if (vsSource.empty()) { vsSource = std::string(UnlitVertexShaderSource); }
            if (fsSource.empty()) { fsSource = std::string(UnlitFragmentShaderSource); }

            auto vs = rhi_ref.createShader({ .stage = ShaderStage::Vertex,   .source = vsSource });
            auto fs = rhi_ref.createShader({ .stage = ShaderStage::Fragment, .source = fsSource });
            auto pipeline = rhi_ref.createPipeline({
                .vertexShader   = vs,
                .fragmentShader = fs,
                .depthTest      = true,
                .depthWrite     = true,
                .blending       = false
            });

            if (pipeline.index != 0) {
                rhi_ref.destroyPipeline({ unlitPipeline_ });
                unlitPipeline_ = pipeline.index;
                NOX_LOG_INFO("Unlit pipeline reloaded");
            }
        }
    }

} // namespace Nox

// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/core/Engine.hpp>
#include <NoxEngine/core/Logger.hpp>
#include <NoxEngine/renderer/Renderer.hpp>
#include <NoxEngine/renderer/RHI.hpp>
#include <NoxEngine/scene/Geometry.hpp>
#include <NoxEngine/scene/Light.hpp>
#include <NoxEngine/scene/Mesh.hpp>

#include <GL/glew.h>
#include <SDL3/SDL.h>

#include <stdexcept>

namespace Nox {

    // ── Embedded shaders ───────────────────────────────────────────

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

    uniform vec3  uObjectColor;
    uniform vec3  uLightDir;
    uniform vec3  uLightColor;
    uniform float uLightIntensity;
    uniform vec3  uAmbientColor;
    uniform float uAmbientIntensity;
    uniform vec3  uCameraPos;

    out vec4 FragColor;

    void main() {
        vec3 N = normalize(vNormal);
        vec3 L = normalize(-uLightDir);

        // Diffuse (Lambertian)
        float diff = max(dot(N, L), 0.0);

        // Specular (Blinn-Phong)
        vec3 V = normalize(uCameraPos - vWorldPos);
        vec3 H = normalize(L + V);
        float spec = pow(max(dot(N, H), 0.0), 32.0);

        vec3 ambient  = uAmbientColor * uAmbientIntensity;
        vec3 diffuse  = uLightColor * uLightIntensity * diff;
        vec3 specular = uLightColor * uLightIntensity * spec * 0.5;

        vec3 result = (ambient + diffuse + specular) * uObjectColor;
        FragColor = vec4(result, 1.0);
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
        });

        if (!config.vsync) {
            SDL_GL_SetSwapInterval(0);
        }

        // Create default pipeline (Blinn-Phong)
        auto& rhi_ref = renderer_->rhi();
        auto vs = rhi_ref.createShader({ .stage = ShaderStage::Vertex,   .source = VertexShaderSource });
        auto fs = rhi_ref.createShader({ .stage = ShaderStage::Fragment, .source = FragmentShaderSource });
        auto pipeline = rhi_ref.createPipeline({
            .vertexShader   = vs,
            .fragmentShader = fs,
            .depthTest      = true,
            .depthWrite     = true,
            .blending       = false
        });
        defaultPipeline_ = pipeline.index;
        pipelineReady_ = true;

        NOX_LOG_INFO("Engine initialized ({}x{})", config.width, config.height);
    }

    Engine::~Engine() {
        // Renderer must be destroyed before window (GL context)
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

            loopFn(dt);

            window_->swapBuffers();

            // FPS logging
            ++frameCount;
            fpsTimer += dt;
            if (fpsTimer >= 1.0f) {
                NOX_LOG_INFO("FPS: {}", frameCount);
                frameCount = 0;
                fpsTimer -= 1.0f;
            }
        }
    }

    void Engine::render(Scene3D& scene, PerspectiveCamera& camera) {
        auto& rhi_ref = renderer_->rhi();

        // Update camera aspect on each frame (in case of resize)
        camera.setAspect(window_->aspect());

        // Upload meshes that aren't on the GPU yet
        for (const auto& mesh : scene.meshes()) {
            if (!mesh->gpuReady()) {
                uploadMesh(*mesh);
            }
        }

        // Find lights for uniforms
        Math::Vec3 lightDir{ -1.0f, -1.0f, -0.5f };
        Math::Vec3 lightColor{ 1.0f, 1.0f, 1.0f };
        float      lightIntensity = 1.0f;
        Math::Vec3 ambientColor{ 1.0f, 1.0f, 1.0f };
        float      ambientIntensity = 0.05f;

        for (const auto& light : scene.lights()) {
            if (auto* dir = dynamic_cast<DirectionalLight*>(light.get())) {
                lightDir = dir->direction();
                lightColor = { dir->color().r, dir->color().g, dir->color().b };
                lightIntensity = dir->intensity();
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

        PipelineHandle pipeHandle{ defaultPipeline_, 1 };
        cmd->bindPipeline(pipeHandle);

        // Set light uniforms
        cmd->pushConstant("uLightDir", lightDir);
        cmd->pushConstant("uLightColor", lightColor);
        cmd->pushConstant("uLightIntensity", lightIntensity);
        cmd->pushConstant("uAmbientColor", ambientColor);
        cmd->pushConstant("uAmbientIntensity", ambientIntensity);
        cmd->pushConstant("uCameraPos", camera.position());

        // Set camera matrices
        cmd->pushConstant("uView", camera.viewMatrix());
        cmd->pushConstant("uProjection", camera.projectionMatrix());

        // Draw each mesh
        for (const auto& mesh : scene.meshes()) {
            mesh->updateWorldMatrix();

            cmd->pushConstant("uModel", mesh->worldMatrix());

            // Object color from material
            const auto& col = mesh->material()->color();
            cmd->pushConstant("uObjectColor", Math::Vec3(col.r, col.g, col.b));

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

} // namespace Nox

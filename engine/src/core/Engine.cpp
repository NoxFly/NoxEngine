// Copyright (c) 2026 NoxFly â€” AGPL-3.0

#include <NoxEngine/core/Engine.hpp>
#include <NoxEngine/core/Logger.hpp>
#include <NoxEngine/renderer/Frustum.hpp>
#include <NoxEngine/renderer/PostProcessStack.hpp>
#include <NoxEngine/renderer/RenderContext.hpp>
#include <NoxEngine/renderer/Renderer.hpp>
#include <NoxEngine/renderer/RHI.hpp>
#include <NoxEngine/renderer/TextureLoader.hpp>
#include <NoxEngine/renderer/effects/BloomEffect.hpp>
#include <NoxEngine/renderer/effects/FXAAEffect.hpp>
#include <NoxEngine/renderer/effects/SSAOEffect.hpp>
#include <NoxEngine/scene/Geometry.hpp>
#include <NoxEngine/scene/Light.hpp>
#include <NoxEngine/scene/Mesh.hpp>
#include <NoxEngine/scene/ModelLoader.hpp>

#include "../renderer/OpenGLRHI.hpp"

#include <GL/glew.h>
#include <SDL3/SDL.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

    // â”€â”€ Embedded shaders â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    static constexpr int MaxDirectionalLights = 4;
    static constexpr int MaxPointLights = 8;

    // â”€â”€ Shadow depth vertex shader â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    static constexpr std::string_view ShadowVertexSource = R"glsl(
    #version 460 core
    layout(location = 0) in vec3 aPosition;
    uniform mat4 uLightSpaceMatrix;
    uniform mat4 uModel;
    void main() {
        gl_Position = uLightSpaceMatrix * uModel * vec4(aPosition, 1.0);
    }
    )glsl";

    static constexpr std::string_view ShadowFragmentSource = R"glsl(
    #version 460 core
    void main() {
        // Depth written automatically
    }
    )glsl";

    // â”€â”€ Point light shadow (6-pass, linear depth) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    static constexpr std::string_view PointShadowVertexSource = R"glsl(
    #version 460 core
    layout(location = 0) in vec3 aPosition;
    uniform mat4 uLightSpaceMatrix;
    uniform mat4 uModel;
    out vec3 vWorldPos;
    void main() {
        vec4 wp = uModel * vec4(aPosition, 1.0);
        vWorldPos = wp.xyz;
        gl_Position = uLightSpaceMatrix * wp;
    }
    )glsl";

    static constexpr std::string_view PointShadowFragmentSource = R"glsl(
    #version 460 core
    in vec3 vWorldPos;
    uniform vec3 uLightPos;
    uniform float uFarPlane;
    void main() {
        float dist = length(vWorldPos - uLightPos);
        gl_FragDepth = dist / uFarPlane;
    }
    )glsl";

    // â”€â”€ PBR lit vertex shader â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    static constexpr std::string_view VertexShaderSource = R"glsl(
    #version 460 core

    layout(location = 0) in vec3 aPosition;
    layout(location = 1) in vec3 aNormal;
    layout(location = 2) in vec2 aUV;
    layout(location = 3) in vec3 aTangent;

    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;
    uniform mat4 uLightSpaceMatrix;

    out vec3 vWorldPos;
    out vec3 vNormal;
    out vec2 vUV;
    out vec3 vTangent;
    out vec4 vLightSpacePos;

    void main() {
        vec4 worldPos = uModel * vec4(aPosition, 1.0);
        vWorldPos = worldPos.xyz;
        mat3 normalMatrix = mat3(transpose(inverse(uModel)));
        vNormal   = normalMatrix * aNormal;
        vTangent  = normalMatrix * aTangent;
        vUV       = aUV;
        vLightSpacePos = uLightSpaceMatrix * worldPos;
        gl_Position = uProjection * uView * worldPos;
    }
    )glsl";

    static constexpr std::string_view FragmentShaderSource = R"glsl(
    #version 460 core

    in vec3 vWorldPos;
    in vec3 vNormal;
    in vec2 vUV;
    in vec3 vTangent;
    in vec4 vLightSpacePos;

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
    uniform float uRoughness;
    uniform float uMetallic;

    // Textures
    uniform bool uHasAlbedoTex;
    uniform sampler2D uAlbedoTex;
    uniform bool uHasNormalTex;
    uniform sampler2D uNormalTex;
    uniform bool uHasEmissiveTex;
    uniform sampler2D uEmissiveTex;
    uniform vec3  uEmissiveColor;
    uniform float uEmissiveIntensity;

    // Shadow map (directional)
    uniform sampler2D uShadowMap;
    uniform bool uHasShadowMap;

    // Point light shadow cubemaps
    uniform samplerCube uPointShadowMaps[4];
    uniform int  uNumShadowPointLights;
    uniform float uPointLightFarPlane;

    layout(location = 0) out vec4 FragColor;

    const float PI = 3.14159265359;

    // â”€â”€ PBR functions â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    float distributionGGX(vec3 N, vec3 H, float roughness) {
        float a  = roughness * roughness;
        float a2 = a * a;
        float NdotH  = max(dot(N, H), 0.0);
        float NdotH2 = NdotH * NdotH;
        float denom  = NdotH2 * (a2 - 1.0) + 1.0;
        return a2 / (PI * denom * denom);
    }

    float geometrySchlickGGX(float NdotV, float roughness) {
        float r = roughness + 1.0;
        float k = (r * r) / 8.0;
        return NdotV / (NdotV * (1.0 - k) + k);
    }

    float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
        float NdotV = max(dot(N, V), 0.0);
        float NdotL = max(dot(N, L), 0.0);
        return geometrySchlickGGX(NdotV, roughness) * geometrySchlickGGX(NdotL, roughness);
    }

    vec3 fresnelSchlick(float cosTheta, vec3 F0) {
        return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
    }

    // â”€â”€ Shadow calculation â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    float calcDirShadow(vec4 lsPos) {
        vec3 projCoords = lsPos.xyz / lsPos.w;
        projCoords = projCoords * 0.5 + 0.5;
        if (projCoords.z > 1.0) return 0.0;

        float currentDepth = projCoords.z;
        float bias = 0.005;

        // PCF 3x3
        float shadow = 0.0;
        vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                float pcfDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
            }
        }
        return shadow / 9.0;
    }

    float calcPointShadow(int idx, vec3 fragToLight, float currentDist) {
        float closestDepth = texture(uPointShadowMaps[idx], fragToLight).r;
        closestDepth *= uPointLightFarPlane;
        float bias = 0.05;
        return currentDist - bias > closestDepth ? 1.0 : 0.0;
    }

    // â”€â”€ Main â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    void main() {
        // Normal mapping
        vec3 N = normalize(vNormal);
        if (uHasNormalTex) {
            vec3 T = normalize(vTangent);
            T = normalize(T - dot(T, N) * N);
            vec3 B = cross(N, T);
            mat3 TBN = mat3(T, B, N);
            vec3 normalSample = texture(uNormalTex, vUV).rgb * 2.0 - 1.0;
            N = normalize(TBN * normalSample);
        }

        vec3 V = normalize(uCameraPos - vWorldPos);

        vec3 albedo = uObjectColor;
        if (uHasAlbedoTex) {
            albedo *= texture(uAlbedoTex, vUV).rgb;
        }

        float roughness = clamp(uRoughness, 0.04, 1.0);
        float metallic  = clamp(uMetallic,  0.0,  1.0);

        // F0: base reflectivity (dielectric = 0.04, metallic = albedo)
        vec3 F0 = mix(vec3(0.04), albedo, metallic);

        // Directional shadow
        float dirShadow = 0.0;
        if (uHasShadowMap) {
            dirShadow = calcDirShadow(vLightSpacePos);
        }

        vec3 Lo = vec3(0.0);

        // â”€â”€ Directional lights â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
        for (int i = 0; i < uNumDirLights; ++i) {
            vec3 L = normalize(-uDirLights[i].direction);
            vec3 H = normalize(V + L);
            vec3 radiance = uDirLights[i].color * uDirLights[i].intensity;

            float NDF = distributionGGX(N, H, roughness);
            float G   = geometrySmith(N, V, L, roughness);
            vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

            vec3 numerator = NDF * G * F;
            float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
            vec3 specular = numerator / denom;

            vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
            float NdotL = max(dot(N, L), 0.0);

            float shadowFactor = (i == 0) ? (1.0 - dirShadow) : 1.0;
            Lo += (kD * albedo / PI + specular) * radiance * NdotL * shadowFactor;
        }

        // â”€â”€ Point lights â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
        for (int i = 0; i < uNumPointLights; ++i) {
            vec3 toLight = uPointLights[i].position - vWorldPos;
            float dist = length(toLight);
            vec3 L = toLight / max(dist, 0.001);
            vec3 H = normalize(V + L);

            float attenuation = 1.0 / (1.0 + dist * dist / max(uPointLights[i].range * uPointLights[i].range, 0.001));
            vec3 radiance = uPointLights[i].color * uPointLights[i].intensity * attenuation;

            float NDF = distributionGGX(N, H, roughness);
            float G   = geometrySmith(N, V, L, roughness);
            vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

            vec3 numerator = NDF * G * F;
            float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
            vec3 specular = numerator / denom;

            vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
            float NdotL = max(dot(N, L), 0.0);

            float shadowFactor = 1.0;
            if (i < uNumShadowPointLights) {
                shadowFactor = 1.0 - calcPointShadow(i, -L, dist);
            }

            Lo += (kD * albedo / PI + specular) * radiance * NdotL * shadowFactor;
        }

        // Ambient (simple IBL approximation)
        vec3 ambient = uAmbientColor * uAmbientIntensity * albedo;

        vec3 color = ambient + Lo;

        // Emissive
        vec3 emissive = uEmissiveColor * uEmissiveIntensity;
        if (uHasEmissiveTex) {
            emissive *= texture(uEmissiveTex, vUV).rgb;
        }
        color += emissive;

        FragColor = vec4(color, 1.0);
    }
    )glsl";

    // â”€â”€ Unlit shaders â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    static constexpr std::string_view UnlitVertexShaderSource = R"glsl(
    #version 460 core

    layout(location = 0) in vec3 aPosition;
    layout(location = 1) in vec3 aNormal;
    layout(location = 2) in vec2 aUV;
    layout(location = 3) in vec3 aTangent;

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

    // â”€â”€ Tone mapping (fullscreen quad) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    static constexpr std::string_view ToneMapVertexSource = R"glsl(
    #version 460 core
    layout(location = 0) in vec2 aPosition;
    layout(location = 1) in vec2 aUV;
    out vec2 vUV;
    void main() {
        vUV = aUV;
        gl_Position = vec4(aPosition, 0.0, 1.0);
    }
    )glsl";

    static constexpr std::string_view ToneMapFragmentSource = R"glsl(
    #version 460 core
    in vec2 vUV;
    uniform sampler2D uHDRBuffer;
    uniform float uExposure;
    out vec4 FragColor;
    void main() {
        vec3 hdrColor = texture(uHDRBuffer, vUV).rgb;

        // Exposure
        vec3 mapped = vec3(1.0) - exp(-hdrColor * uExposure);

        // ACES tone mapping
        // (simple approximation)
        // mapped = (mapped * (2.51 * mapped + 0.03)) / (mapped * (2.43 * mapped + 0.59) + 0.14);

        // Gamma correction
        mapped = pow(mapped, vec3(1.0 / 2.2));

        FragColor = vec4(mapped, 1.0);
    }
    )glsl";

    // â”€â”€ Engine â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    Engine::Engine(const EngineConfig& config) {
        window_ = std::make_unique<Window>(config.title, config.width, config.height);

        auto rhi = createOpenGLRHI();
        renderer_ = std::make_unique<Renderer>(std::move(rhi));
        renderer_->onResize(config.width, config.height);

        // Handle resize
        window_->onResize.connect([this](int w, int h) {
            renderer_->onResize(w, h);
            glViewport(0, 0, w, h);
            createHDRResources(w, h);
        });

        if (!config.vsync) {
            SDL_GL_SetSwapInterval(0);
        }

        // Create lit pipeline (PBR Cook-Torrance)
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
            renderCtx_.litPipeline = pipeline.index;
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
            renderCtx_.unlitPipeline = pipeline.index;
        }

        // Create shadow depth pipeline (directional)
        {
            auto vs = rhi_ref.createShader({ .stage = ShaderStage::Vertex,   .source = ShadowVertexSource });
            auto fs = rhi_ref.createShader({ .stage = ShaderStage::Fragment, .source = ShadowFragmentSource });
            auto pipeline = rhi_ref.createPipeline({
                .vertexShader   = vs,
                .fragmentShader = fs,
                .depthTest      = true,
                .depthWrite     = true,
                .blending       = false
            });
            renderCtx_.shadowPipeline = pipeline.index;
        }

        // Create tone mapping pipeline
        {
            auto vs = rhi_ref.createShader({ .stage = ShaderStage::Vertex,   .source = ToneMapVertexSource });
            auto fs = rhi_ref.createShader({ .stage = ShaderStage::Fragment, .source = ToneMapFragmentSource });
            auto pipeline = rhi_ref.createPipeline({
                .vertexShader   = vs,
                .fragmentShader = fs,
                .depthTest      = false,
                .depthWrite     = false,
                .blending       = false
            });
            renderCtx_.toneMapPipeline = pipeline.index;
        }

        // Create point shadow pipeline (linear depth output)
        {
            auto vs = rhi_ref.createShader({ .stage = ShaderStage::Vertex,   .source = PointShadowVertexSource });
            auto fs = rhi_ref.createShader({ .stage = ShaderStage::Fragment, .source = PointShadowFragmentSource });
            auto pipeline = rhi_ref.createPipeline({
                .vertexShader   = vs,
                .fragmentShader = fs,
                .depthTest      = true,
                .depthWrite     = true,
                .blending       = false
            });
            renderCtx_.pointShadowPipeline = pipeline.index;
        }

        // Create shadow map resources
        createShadowResources();

        // Create HDR framebuffer
        createHDRResources(config.width, config.height);

        // Initialize post-processing stack with default effects
        // TODO: Effects start disabled for v0.4 baseline testing
        renderCtx_.postProcessStack.addEffect(std::make_unique<SSAOEffect>());
        renderCtx_.postProcessStack.addEffect(std::make_unique<BloomEffect>());
        renderCtx_.postProcessStack.addEffect(std::make_unique<FXAAEffect>());
        renderCtx_.postProcessStack.init(config.width, config.height);

        // DISABLE ALL EFFECTS BY DEFAULT FOR DEBUGGING
        for (auto& effect : const_cast<std::vector<std::unique_ptr<PostProcessEffect>>&>(renderCtx_.postProcessStack.effects())) {
            effect->setEnabled(false);
        }

        renderCtx_.pipelineReady = true;

        debugOverlay_.init(*this);

        NOX_LOG_INFO("Engine initialized ({}x{}) â€” PBR rendering", config.width, config.height);
    }

    Engine::~Engine() {
        debugOverlay_.shutdown();
        // renderCtx_ is the last declared member, so it's destroyed first
        // (reverse declaration order). This ensures GPU cleanup happens
        // while the GL context (window_) is still alive.
    }

    float Engine::aspect() const {
        return window_->aspect();
    }

    void Engine::run(std::function<void(float dt)> loopFn) {
        uint64_t lastTicks = SDL_GetPerformanceCounter();
        uint64_t freq = SDL_GetPerformanceFrequency();

        float fpsTimer = 0.0f;
        int   frameCount = 0;

        running_ = true;
        while (running_ && window_->pollEvents()) {
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
                frameCount = 0;
                fpsTimer -= 1.0f;
            }
        }
    }

    void Engine::stop() {
        running_ = false;
    }

    void Engine::render(Scene3D& scene, PerspectiveCamera& camera) {
        camera.setAspect(window_->aspect());
        renderInternal(scene, camera.viewMatrix(), camera.projectionMatrix(), camera.position());
    }

    template<CameraLike CamT>
    void Engine::render(Scene3D& scene, CamT& camera) {
        renderInternal(scene, camera.viewMatrix(), camera.projectionMatrix(), camera.position());
    }

    // Explicit instantiation for OrthographicCamera
    template void Engine::render<OrthographicCamera>(Scene3D&, OrthographicCamera&);

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
            // Upload textures for this material
            uploadTexture(*mesh);
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



        // Gather lights (using type tags — no RTTI)
        std::vector<const DirectionalLight*> dirLights;
        std::vector<const PointLight*> pointLights;
        Math::Vec3 ambientColor{ 1.0f, 1.0f, 1.0f };
        float      ambientIntensity = 0.05f;

        for (const auto& light : scene.lights()) {
            switch (light->objectType()) {
                case SceneObjectType::DirectionalLight:
                    if (static_cast<int>(dirLights.size()) < MaxDirectionalLights) {
                        dirLights.push_back(static_cast<const DirectionalLight*>(light.get()));
                    }
                    break;
                case SceneObjectType::PointLight:
                    if (static_cast<int>(pointLights.size()) < MaxPointLights) {
                        pointLights.push_back(static_cast<const PointLight*>(light.get()));
                    }
                    break;
                case SceneObjectType::AmbientLight: {
                    const auto* amb = static_cast<const AmbientLight*>(light.get());
                    ambientColor = { amb->color().r, amb->color().g, amb->color().b };
                    ambientIntensity = amb->intensity();
                    break;
                }
                default:
                    break;
            }
        }

        // Begin rendering
        auto* cmd = rhi_ref.beginFrame();

        // â”€â”€ Shadow pass (directional light 0) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
        if (!dirLights.empty()) {
            const auto& dir = dirLights[0]->direction();
            float shadowExtent = 20.0f;
            Math::Mat4 lightProj = glm::ortho(-shadowExtent, shadowExtent, -shadowExtent, shadowExtent, 0.1f, 50.0f);
            Math::Vec3 lightPos = -glm::normalize(dir) * 20.0f;
            Math::Mat4 lightView = glm::lookAt(lightPos, Math::Vec3(0.0f), Math::Vec3(0.0f, 1.0f, 0.0f));
            renderCtx_.lightSpaceMatrix = lightProj * lightView;

            glBindFramebuffer(GL_FRAMEBUFFER, renderCtx_.shadowFBO);
            glViewport(0, 0, RenderContext::ShadowMapSize, RenderContext::ShadowMapSize);
            glClear(GL_DEPTH_BUFFER_BIT);

            PipelineHandle shadowPipe{ renderCtx_.shadowPipeline, 1 };

            for (const auto& entry : visibleMeshes) {
                auto* mesh = entry.mesh;
                if (!mesh->material()->isLit()) { continue; }

                auto& pipePool = static_cast<OpenGLRHI&>(rhi_ref).pipelinePool();
                HandlePool<GLPipelineData>::Handle ph{ renderCtx_.shadowPipeline, 1 };
                auto* pipeData = pipePool.get(ph);
                if (pipeData) {
                    glUseProgram(pipeData->program);
                    GLint locLSM = glGetUniformLocation(pipeData->program, "uLightSpaceMatrix");
                    GLint locModel = glGetUniformLocation(pipeData->program, "uModel");
                    glUniformMatrix4fv(locLSM, 1, GL_FALSE, glm::value_ptr(renderCtx_.lightSpaceMatrix));
                    glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(mesh->worldMatrix()));
                }

                const auto& gpu = mesh->gpuData();
                glBindVertexArray(gpu.vao);
                glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(gpu.indexCount), GL_UNSIGNED_INT, nullptr);
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // â”€â”€ Point light shadow pass (6-pass per cubemap face) â”€â”€â”€â”€â”€â”€â”€
        renderCtx_.numShadowPointLights = std::min(static_cast<int>(pointLights.size()), RenderContext::MaxShadowPointLights);
        float pointFarPlane = 25.0f;

        // Reuse the point shadow pipeline (linear depth output)
        auto& ptShadowPipePool = static_cast<OpenGLRHI&>(rhi_ref).pipelinePool();
        HandlePool<GLPipelineData>::Handle ptShadowPH{ renderCtx_.pointShadowPipeline, 1 };
        auto* ptShadowPipeData = ptShadowPipePool.get(ptShadowPH);

        for (int li = 0; li < renderCtx_.numShadowPointLights; ++li) {
            const auto& lightPos = pointLights[static_cast<size_t>(li)]->transform().position();
            Math::Mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, pointFarPlane);

            Math::Mat4 shadowTransforms[6] = {
                shadowProj * glm::lookAt(lightPos, lightPos + Math::Vec3( 1, 0, 0), Math::Vec3(0,-1, 0)),
                shadowProj * glm::lookAt(lightPos, lightPos + Math::Vec3(-1, 0, 0), Math::Vec3(0,-1, 0)),
                shadowProj * glm::lookAt(lightPos, lightPos + Math::Vec3( 0, 1, 0), Math::Vec3(0, 0, 1)),
                shadowProj * glm::lookAt(lightPos, lightPos + Math::Vec3( 0,-1, 0), Math::Vec3(0, 0,-1)),
                shadowProj * glm::lookAt(lightPos, lightPos + Math::Vec3( 0, 0, 1), Math::Vec3(0,-1, 0)),
                shadowProj * glm::lookAt(lightPos, lightPos + Math::Vec3( 0, 0,-1), Math::Vec3(0,-1, 0)),
            };

            if (ptShadowPipeData) {
                glUseProgram(ptShadowPipeData->program);
                GLint locLSM = glGetUniformLocation(ptShadowPipeData->program, "uLightSpaceMatrix");
                GLint locModel = glGetUniformLocation(ptShadowPipeData->program, "uModel");
                GLint locLP = glGetUniformLocation(ptShadowPipeData->program, "uLightPos");
                GLint locFP = glGetUniformLocation(ptShadowPipeData->program, "uFarPlane");
                glUniform3fv(locLP, 1, glm::value_ptr(lightPos));
                glUniform1f(locFP, pointFarPlane);

                for (int face = 0; face < 6; ++face) {
                    // Attach the cubemap face to the FBO
                    glNamedFramebufferTextureLayer(renderCtx_.pointShadowFBOs[li], GL_DEPTH_ATTACHMENT,
                                                    renderCtx_.pointShadowCubemaps[li], 0, face);
                    glBindFramebuffer(GL_FRAMEBUFFER, renderCtx_.pointShadowFBOs[li]);
                    glViewport(0, 0, RenderContext::PointShadowMapSize, RenderContext::PointShadowMapSize);
                    glClear(GL_DEPTH_BUFFER_BIT);

                    glUniformMatrix4fv(locLSM, 1, GL_FALSE, glm::value_ptr(shadowTransforms[face]));

                    for (const auto& entry : visibleMeshes) {
                        auto* mesh = entry.mesh;
                        if (!mesh->material()->isLit()) { continue; }
                        glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(mesh->worldMatrix()));
                        const auto& gpu = mesh->gpuData();
                        glBindVertexArray(gpu.vao);
                        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(gpu.indexCount), GL_UNSIGNED_INT, nullptr);
                    }
                }
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // â”€â”€ HDR main pass â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
        glBindFramebuffer(GL_FRAMEBUFFER, renderCtx_.hdrFBO);
        auto [w, h] = window_->size();
        glViewport(0, 0, w, h);
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);



        // Draw each visible mesh
        int meshesDrawn = 0;
        for (const auto& entry : visibleMeshes) {
            auto* mesh = entry.mesh;

            // Choose pipeline based on material
            uint32_t pipeIdx = mesh->material()->isLit() ? renderCtx_.litPipeline : renderCtx_.unlitPipeline;
            PipelineHandle pipeHandle{ pipeIdx, 1 };
            cmd->bindPipeline(pipeHandle);

            // Camera matrices
            cmd->pushConstant("uView", viewMatrix);
            cmd->pushConstant("uProjection", projMatrix);

            if (mesh->material()->isLit()) {
                // Light space matrix for shadow
                cmd->pushConstant("uLightSpaceMatrix", renderCtx_.lightSpaceMatrix);

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

                // PBR material uniforms
                cmd->pushConstant("uRoughness", mesh->material()->roughness());
                cmd->pushConstant("uMetallic", mesh->material()->metallic());

                // Emissive
                const auto& ec = mesh->material()->emissiveColor();
                cmd->pushConstant("uEmissiveColor", Math::Vec3(ec.r, ec.g, ec.b));
                cmd->pushConstant("uEmissiveIntensity", mesh->material()->emissiveIntensity());

                // Shadow map binding (texture unit 3)
                glBindTextureUnit(3, renderCtx_.shadowDepthTex);
                cmd->pushConstant("uShadowMap", 3);
                cmd->pushConstant("uHasShadowMap", dirLights.empty() ? 0 : 1);

                // Point shadow cubemaps (texture units 4-7)
                cmd->pushConstant("uNumShadowPointLights", renderCtx_.numShadowPointLights);
                cmd->pushConstant("uPointLightFarPlane", pointFarPlane);
                for (int i = 0; i < renderCtx_.numShadowPointLights; ++i) {
                    glBindTextureUnit(static_cast<GLuint>(4 + i), renderCtx_.pointShadowCubemaps[i]);
                    cmd->pushConstant("uPointShadowMaps[" + std::to_string(i) + "]", 4 + i);
                }

                // Normal map (texture unit 2)
                if (mesh->material()->hasNormalTexture()) {
                    glBindTextureUnit(2, mesh->material()->normalTextureId());
                    cmd->pushConstant("uHasNormalTex", 1);
                    cmd->pushConstant("uNormalTex", 2);
                } else {
                    cmd->pushConstant("uHasNormalTex", 0);
                }

                // Emissive map (texture unit 8)
                if (mesh->material()->hasEmissiveTexture()) {
                    glBindTextureUnit(8, mesh->material()->emissiveTextureId());
                    cmd->pushConstant("uHasEmissiveTex", 1);
                    cmd->pushConstant("uEmissiveTex", 8);
                } else {
                    cmd->pushConstant("uHasEmissiveTex", 0);
                }
            }

            cmd->pushConstant("uModel", mesh->worldMatrix());

            // Object color from material
            const auto& col = mesh->material()->color();
            cmd->pushConstant("uObjectColor", Math::Vec3(col.r, col.g, col.b));

            // Albedo texture (texture unit 0)
            if (mesh->material()->hasAlbedoTexture()) {
                glBindTextureUnit(0, mesh->material()->albedoTextureId());
                cmd->pushConstant("uHasAlbedoTex", 1);
                cmd->pushConstant("uAlbedoTex", 0);
            }
            else {
                cmd->pushConstant("uHasAlbedoTex", 0);
            }

            // Bind mesh GPU resources directly
            const auto& gpu = mesh->gpuData();
            glBindVertexArray(gpu.vao);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(gpu.indexCount),
                        GL_UNSIGNED_INT, nullptr);
            ++meshesDrawn;
        }



        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // â”€â”€ Tone mapping pass â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
        renderToneMapPass();
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

        // Tangent (location 3)
        glEnableVertexArrayAttrib(vao, 3);
        glVertexArrayAttribFormat(vao, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent));
        glVertexArrayAttribBinding(vao, 3, 0);

        mesh.gpuData() = Mesh::GpuData{
            .vao        = vao,
            .vbo        = vbo,
            .ibo        = ibo,
            .indexCount = static_cast<uint32_t>(idxs.size())
        };
        mesh.markGpuReady(true);
    }

    void Engine::uploadTexture(Mesh& mesh) {
        auto& mat = *mesh.material();

        // Albedo texture
        if (!mat.albedoMapPath().empty() && !mat.hasAlbedoTexture()) {
            uint32_t texId = loadTexture(mat.albedoMapPath());
            if (texId != 0) {
                mat.setAlbedoTextureId(texId);
            }
        }

        // Normal map
        if (!mat.normalMapPath().empty() && !mat.hasNormalTexture()) {
            uint32_t texId = loadTexture(mat.normalMapPath());
            if (texId != 0) {
                mat.setNormalTextureId(texId);
            }
        }

        // Emissive map
        if (!mat.emissiveMapPath().empty() && !mat.hasEmissiveTexture()) {
            uint32_t texId = loadTexture(mat.emissiveMapPath());
            if (texId != 0) {
                mat.setEmissiveTextureId(texId);
            }
        }
    }

    uint32_t Engine::loadTexture(const std::filesystem::path& path) {
        if (textureCache_.contains(path)) {
            return textureCache_.get(path);
        }

        auto texData = TextureLoader::load(path.string());
        if (!texData.pixels) { return 0; }

        GLuint texId = 0;
        glCreateTextures(GL_TEXTURE_2D, 1, &texId);
        glTextureStorage2D(texId, 1, GL_RGBA8, texData.width, texData.height);
        glTextureSubImage2D(texId, 0, 0, 0, texData.width, texData.height,
                            GL_RGBA, GL_UNSIGNED_BYTE, texData.pixels);

        glTextureParameteri(texId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(texId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(texId, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(texId, GL_TEXTURE_WRAP_T, GL_REPEAT);

        textureCache_.store(path, texId);
        TextureLoader::free(texData);
        return texId;
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
                rhi_ref.destroyPipeline({ renderCtx_.litPipeline });
                renderCtx_.litPipeline = pipeline.index;
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
                rhi_ref.destroyPipeline({ renderCtx_.unlitPipeline });
                renderCtx_.unlitPipeline = pipeline.index;
                NOX_LOG_INFO("Unlit pipeline reloaded");
            }
        }
    }

    void Engine::createShadowResources() {
        // â”€â”€ Directional shadow map â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
        glCreateTextures(GL_TEXTURE_2D, 1, &renderCtx_.shadowDepthTex);
        glTextureStorage2D(renderCtx_.shadowDepthTex, 1, GL_DEPTH_COMPONENT24, RenderContext::ShadowMapSize, RenderContext::ShadowMapSize);
        glTextureParameteri(renderCtx_.shadowDepthTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(renderCtx_.shadowDepthTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(renderCtx_.shadowDepthTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTextureParameteri(renderCtx_.shadowDepthTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTextureParameterfv(renderCtx_.shadowDepthTex, GL_TEXTURE_BORDER_COLOR, borderColor);
        glTextureParameteri(renderCtx_.shadowDepthTex, GL_TEXTURE_COMPARE_MODE, GL_NONE);

        glCreateFramebuffers(1, &renderCtx_.shadowFBO);
        glNamedFramebufferTexture(renderCtx_.shadowFBO, GL_DEPTH_ATTACHMENT, renderCtx_.shadowDepthTex, 0);
        glNamedFramebufferDrawBuffer(renderCtx_.shadowFBO, GL_NONE);
        glNamedFramebufferReadBuffer(renderCtx_.shadowFBO, GL_NONE);

        // â”€â”€ Point light shadow cubemaps â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
        for (int i = 0; i < RenderContext::MaxShadowPointLights; ++i) {
            glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &renderCtx_.pointShadowCubemaps[i]);
            glTextureStorage2D(renderCtx_.pointShadowCubemaps[i], 1, GL_DEPTH_COMPONENT24,
                               RenderContext::PointShadowMapSize, RenderContext::PointShadowMapSize);
            glTextureParameteri(renderCtx_.pointShadowCubemaps[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(renderCtx_.pointShadowCubemaps[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(renderCtx_.pointShadowCubemaps[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(renderCtx_.pointShadowCubemaps[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTextureParameteri(renderCtx_.pointShadowCubemaps[i], GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            glCreateFramebuffers(1, &renderCtx_.pointShadowFBOs[i]);
            glNamedFramebufferTexture(renderCtx_.pointShadowFBOs[i], GL_DEPTH_ATTACHMENT,
                                      renderCtx_.pointShadowCubemaps[i], 0);
            glNamedFramebufferDrawBuffer(renderCtx_.pointShadowFBOs[i], GL_NONE);
            glNamedFramebufferReadBuffer(renderCtx_.pointShadowFBOs[i], GL_NONE);
        }

        NOX_LOG_INFO("Shadow resources created (dir {}x{}, point {}x{})",
                     RenderContext::ShadowMapSize, RenderContext::ShadowMapSize, RenderContext::PointShadowMapSize, RenderContext::PointShadowMapSize);
    }

    void Engine::createHDRResources(int width, int height) {
        if (width == renderCtx_.hdrWidth && height == renderCtx_.hdrHeight && renderCtx_.hdrFBO != 0) {
            return;
        }

        // Destroy previous resources if resizing
        if (renderCtx_.hdrFBO != 0) {
            glDeleteFramebuffers(1, &renderCtx_.hdrFBO);
            glDeleteTextures(1, &renderCtx_.hdrColorTex);
            glDeleteTextures(1, &renderCtx_.hdrDepthTex);
        }

        renderCtx_.hdrWidth  = width;
        renderCtx_.hdrHeight = height;

        // Color texture (RGBA16F for HDR)
        glCreateTextures(GL_TEXTURE_2D, 1, &renderCtx_.hdrColorTex);
        glTextureStorage2D(renderCtx_.hdrColorTex, 1, GL_RGBA16F, width, height);
        glTextureParameteri(renderCtx_.hdrColorTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(renderCtx_.hdrColorTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Depth texture (for SSAO and other effects that need depth)
        glCreateTextures(GL_TEXTURE_2D, 1, &renderCtx_.hdrDepthTex);
        glTextureStorage2D(renderCtx_.hdrDepthTex, 1, GL_DEPTH24_STENCIL8, width, height);
        glTextureParameteri(renderCtx_.hdrDepthTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(renderCtx_.hdrDepthTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Framebuffer
        glCreateFramebuffers(1, &renderCtx_.hdrFBO);
        glNamedFramebufferTexture(renderCtx_.hdrFBO, GL_COLOR_ATTACHMENT0, renderCtx_.hdrColorTex, 0);
        glNamedFramebufferTexture(renderCtx_.hdrFBO, GL_DEPTH_STENCIL_ATTACHMENT, renderCtx_.hdrDepthTex, 0);

        // Create screen quad VAO (if not already created)
        if (renderCtx_.screenQuadVAO == 0) {
            float quadVertices[] = {
                -1.0f, -1.0f,  0.0f, 0.0f,
                 1.0f, -1.0f,  1.0f, 0.0f,
                -1.0f,  1.0f,  0.0f, 1.0f,
                 1.0f,  1.0f,  1.0f, 1.0f,
            };

            glCreateVertexArrays(1, &renderCtx_.screenQuadVAO);
            glCreateBuffers(1, &renderCtx_.screenQuadVBO);
            glNamedBufferStorage(renderCtx_.screenQuadVBO, sizeof(quadVertices), quadVertices, 0);

            glEnableVertexArrayAttrib(renderCtx_.screenQuadVAO, 0);
            glVertexArrayAttribFormat(renderCtx_.screenQuadVAO, 0, 2, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(renderCtx_.screenQuadVAO, 0, 0);

            glEnableVertexArrayAttrib(renderCtx_.screenQuadVAO, 1);
            glVertexArrayAttribFormat(renderCtx_.screenQuadVAO, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float));
            glVertexArrayAttribBinding(renderCtx_.screenQuadVAO, 1, 0);

            glVertexArrayVertexBuffer(renderCtx_.screenQuadVAO, 0, renderCtx_.screenQuadVBO, 0, 4 * sizeof(float));
        }

        // Resize post-process stack
        renderCtx_.postProcessStack.resize(width, height);

        NOX_LOG_INFO("HDR framebuffer created ({}x{})", width, height);
    }

    void Engine::renderToneMapPass() {
        auto [w, h] = window_->size();

        // Feed SSAO the depth texture
        auto* ssaoEffect = renderCtx_.postProcessStack.getEffect("SSAO");
        if (ssaoEffect) {
            static_cast<SSAOEffect*>(ssaoEffect)->setDepthTexture(renderCtx_.hdrDepthTex);
        }

        // Apply HDR post-process effects (SSAO, Bloom).
        // FXAA is disabled here â€” it runs after tone mapping on LDR.
        auto* fxaa = renderCtx_.postProcessStack.getEffect("FXAA");
        bool fxaaWasEnabled = fxaa && fxaa->isEnabled();
        if (fxaa) { fxaa->setEnabled(false); }

        uint32_t postProcessedTex = renderCtx_.postProcessStack.apply(renderCtx_.hdrColorTex, w, h);

        if (fxaa) { fxaa->setEnabled(fxaaWasEnabled); }



        // Tone mapping: HDR â†’ default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, w, h);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        auto& rhi_ref = renderer_->rhi();
        auto& pipePool = static_cast<OpenGLRHI&>(rhi_ref).pipelinePool();
        HandlePool<GLPipelineData>::Handle ph{ renderCtx_.toneMapPipeline, 1 };
        auto* pipeData = pipePool.get(ph);

        if (pipeData) {
            glUseProgram(pipeData->program);
            glBindTextureUnit(0, postProcessedTex);
            GLint locBuf = glGetUniformLocation(pipeData->program, "uHDRBuffer");
            glUniform1i(locBuf, 0);
            GLint locExp = glGetUniformLocation(pipeData->program, "uExposure");
            glUniform1f(locExp, renderCtx_.exposure);
            glBindVertexArray(renderCtx_.screenQuadVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        } else {
            NOX_LOG_ERROR("Tone mapping pipeline not found!");
        }

        glEnable(GL_DEPTH_TEST);
    }

} // namespace Nox

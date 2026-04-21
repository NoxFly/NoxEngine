// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/Decal.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>

namespace Nox {

    // ── Decal ──────────────────────────────────────────────────────

    Decal::Decal(float width, float height, float depth)
        : SceneObject("Decal")
        , width_(width)
        , height_(height)
        , depth_(depth) {
    }

    void Decal::setSize(float width, float height, float depth) {
        width_  = width;
        height_ = height;
        depth_  = depth;
    }

    void Decal::setAlbedoMap(const std::string& path) {
        albedoMapPath_ = path;
    }

    void Decal::setNormalMap(const std::string& path) {
        normalMapPath_ = path;
    }

    // ── DecalRenderer ──────────────────────────────────────────────

    static constexpr std::string_view DecalVertexSource = R"glsl(
    #version 460 core

    layout(location = 0) in vec3 aPosition;

    uniform mat4 uMVP;
    uniform mat4 uModel;

    out vec4 vClipPos;
    out vec3 vWorldPos;

    void main() {
        vec4 worldPos = uModel * vec4(aPosition, 1.0);
        vWorldPos = worldPos.xyz;
        gl_Position = uMVP * vec4(aPosition, 1.0);
        vClipPos = gl_Position;
    }
    )glsl";

    static constexpr std::string_view DecalFragmentSource = R"glsl(
    #version 460 core

    in vec4 vClipPos;
    in vec3 vWorldPos;

    uniform sampler2D uDepthTexture;
    uniform sampler2D uGBufferNormal;
    uniform sampler2D uAlbedoMap;
    uniform mat4 uInvModel;
    uniform mat4 uInvViewProj;
    uniform float uOpacity;
    uniform vec2 uViewportSize;

    layout(location = 0) out vec4 outAlbedo;

    vec3 worldPosFromDepth(vec2 uv, float depth) {
        vec4 clipPos = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
        vec4 worldPos = uInvViewProj * clipPos;
        return worldPos.xyz / worldPos.w;
    }

    void main() {
        vec2 screenUV = gl_FragCoord.xy / uViewportSize;
        float depth = texture(uDepthTexture, screenUV).r;

        // Reconstruct world position from depth
        vec3 worldPos = worldPosFromDepth(screenUV, depth);

        // Transform to decal's local space
        vec3 localPos = (uInvModel * vec4(worldPos, 1.0)).xyz;

        // Discard if outside the decal volume [-0.5, 0.5]^3
        if (abs(localPos.x) > 0.5 || abs(localPos.y) > 0.5 || abs(localPos.z) > 0.5) {
            discard;
        }

        // Check surface normal — skip surfaces facing away
        vec3 normal = normalize(texture(uGBufferNormal, screenUV).xyz);
        vec3 decalUp = normalize((uInvModel * vec4(0, 1, 0, 0)).xyz);
        if (dot(normal, decalUp) < 0.1) {
            discard;
        }

        // Sample decal texture using local XZ as UV
        vec2 decalUV = localPos.xz + 0.5;
        vec4 decalColor = texture(uAlbedoMap, decalUV);

        // Edge fade
        vec2 edgeDist = abs(localPos.xz) * 2.0;
        float edgeFade = (1.0 - smoothstep(0.8, 1.0, edgeDist.x))
                       * (1.0 - smoothstep(0.8, 1.0, edgeDist.y));

        outAlbedo = vec4(decalColor.rgb, decalColor.a * uOpacity * edgeFade);
    }
    )glsl";

    DecalRenderer::~DecalRenderer() {
        if (decalProgram_ != 0) { glDeleteProgram(decalProgram_); }
        if (cubeVAO_ != 0)      { glDeleteVertexArrays(1, &cubeVAO_); }
        if (cubeVBO_ != 0)      { glDeleteBuffers(1, &cubeVBO_); }
        if (cubeIBO_ != 0)      { glDeleteBuffers(1, &cubeIBO_); }
    }

    void DecalRenderer::init() {
        if (initialized_) { return; }

        // Compile decal shader
        auto compileShader = [](GLenum type, std::string_view src) -> GLuint {
            GLuint shader = glCreateShader(type);
            const char* srcPtr = src.data();
            auto len = static_cast<GLint>(src.size());
            glShaderSource(shader, 1, &srcPtr, &len);
            glCompileShader(shader);
            GLint success = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                char log[512]{};
                glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
                NOX_LOG_ERROR("Decal shader error: {}", log);
                glDeleteShader(shader);
                return 0;
            }
            return shader;
        };

        GLuint vs = compileShader(GL_VERTEX_SHADER, DecalVertexSource);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, DecalFragmentSource);

        if (vs == 0 || fs == 0) {
            if (vs) { glDeleteShader(vs); }
            if (fs) { glDeleteShader(fs); }
            return;
        }

        decalProgram_ = glCreateProgram();
        glAttachShader(decalProgram_, vs);
        glAttachShader(decalProgram_, fs);
        glLinkProgram(decalProgram_);
        glDeleteShader(vs);
        glDeleteShader(fs);

        createUnitCube();
        initialized_ = true;
    }

    void DecalRenderer::createUnitCube() {
        // Unit cube vertices [-0.5, 0.5]
        constexpr std::array<float, 24 * 3> vertices = {
            // Front face
            -0.5f, -0.5f,  0.5f,   0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,
            // Back face
            -0.5f, -0.5f, -0.5f,  -0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,   0.5f, -0.5f, -0.5f,
            // Top face
            -0.5f,  0.5f, -0.5f,  -0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,   0.5f,  0.5f, -0.5f,
            // Bottom face
            -0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,
             0.5f, -0.5f,  0.5f,  -0.5f, -0.5f,  0.5f,
            // Right face
             0.5f, -0.5f, -0.5f,   0.5f,  0.5f, -0.5f,
             0.5f,  0.5f,  0.5f,   0.5f, -0.5f,  0.5f,
            // Left face
            -0.5f, -0.5f, -0.5f,  -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,  -0.5f,  0.5f, -0.5f,
        };

        constexpr std::array<uint32_t, 36> indices = {
             0,  1,  2,   2,  3,  0,   // front
             4,  5,  6,   6,  7,  4,   // back
             8,  9, 10,  10, 11,  8,   // top
            12, 13, 14,  14, 15, 12,   // bottom
            16, 17, 18,  18, 19, 16,   // right
            20, 21, 22,  22, 23, 20,   // left
        };

        glCreateVertexArrays(1, &cubeVAO_);
        glCreateBuffers(1, &cubeVBO_);
        glCreateBuffers(1, &cubeIBO_);

        glNamedBufferStorage(cubeVBO_, sizeof(vertices), vertices.data(), 0);
        glNamedBufferStorage(cubeIBO_, sizeof(indices), indices.data(), 0);

        glVertexArrayVertexBuffer(cubeVAO_, 0, cubeVBO_, 0, 3 * sizeof(float));
        glVertexArrayElementBuffer(cubeVAO_, cubeIBO_);

        glEnableVertexArrayAttrib(cubeVAO_, 0);
        glVertexArrayAttribFormat(cubeVAO_, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(cubeVAO_, 0, 0);
    }

    void DecalRenderer::render(const std::vector<std::shared_ptr<Decal>>& decals,
                                const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix,
                                uint32_t gBufferDepthTex, uint32_t gBufferNormalTex,
                                int viewportWidth, int viewportHeight) {
        if (!initialized_ || decals.empty() || decalProgram_ == 0) { return; }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        // Render back faces so the decal works when camera is inside the volume
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        glUseProgram(decalProgram_);

        Math::Mat4 viewProj = projMatrix * viewMatrix;
        Math::Mat4 invViewProj = glm::inverse(viewProj);
        glUniformMatrix4fv(glGetUniformLocation(decalProgram_, "uInvViewProj"),
                           1, GL_FALSE, glm::value_ptr(invViewProj));
        glUniform2f(glGetUniformLocation(decalProgram_, "uViewportSize"),
                    static_cast<float>(viewportWidth), static_cast<float>(viewportHeight));

        glBindTextureUnit(0, gBufferDepthTex);
        glUniform1i(glGetUniformLocation(decalProgram_, "uDepthTexture"), 0);

        glBindTextureUnit(1, gBufferNormalTex);
        glUniform1i(glGetUniformLocation(decalProgram_, "uGBufferNormal"), 1);

        glBindVertexArray(cubeVAO_);

        for (const auto& decal : decals) {
            Math::Mat4 model = decal->worldMatrix();
            // Apply decal size
            model = glm::scale(model, Math::Vec3(decal->width(), decal->height(), decal->depth()));

            Math::Mat4 mvp = viewProj * model;
            Math::Mat4 invModel = glm::inverse(model);

            glUniformMatrix4fv(glGetUniformLocation(decalProgram_, "uMVP"),
                               1, GL_FALSE, glm::value_ptr(mvp));
            glUniformMatrix4fv(glGetUniformLocation(decalProgram_, "uModel"),
                               1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(glGetUniformLocation(decalProgram_, "uInvModel"),
                               1, GL_FALSE, glm::value_ptr(invModel));
            glUniform1f(glGetUniformLocation(decalProgram_, "uOpacity"), decal->opacity());

            // Bind decal albedo texture
            if (decal->gpuData().albedoTexture != 0) {
                glBindTextureUnit(2, decal->gpuData().albedoTexture);
                glUniform1i(glGetUniformLocation(decalProgram_, "uAlbedoMap"), 2);
            }

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        }

        // Restore state
        glCullFace(GL_BACK);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }

} // namespace Nox

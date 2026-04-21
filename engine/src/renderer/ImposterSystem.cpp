// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/ImposterSystem.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace Nox {

    // ── Imposter billboard shader ──────────────────────────────────

    static constexpr std::string_view ImposterVertexSource = R"glsl(
    #version 460 core

    layout(location = 0) in vec3 aPosition;
    layout(location = 1) in vec2 aUV;

    uniform mat4 uViewProj;
    uniform vec3 uBillboardPos;
    uniform float uBillboardSize;
    uniform vec3 uCamRight;
    uniform vec3 uCamUp;
    uniform vec4 uAtlasRect; // x, y, width, height in UV coords

    out vec2 vUV;

    void main() {
        vec3 worldPos = uBillboardPos
                      + uCamRight * aPosition.x * uBillboardSize
                      + uCamUp * aPosition.y * uBillboardSize;

        vUV = uAtlasRect.xy + aUV * uAtlasRect.zw;
        gl_Position = uViewProj * vec4(worldPos, 1.0);
    }
    )glsl";

    static constexpr std::string_view ImposterFragmentSource = R"glsl(
    #version 460 core

    in vec2 vUV;

    uniform sampler2D uAtlas;

    out vec4 FragColor;

    void main() {
        vec4 color = texture(uAtlas, vUV);
        if (color.a < 0.1) discard;
        FragColor = color;
    }
    )glsl";

    ImposterSystem::~ImposterSystem() {
        if (captureFBO_ != 0) { glDeleteFramebuffers(1, &captureFBO_); }
        if (captureRBO_ != 0) { glDeleteRenderbuffers(1, &captureRBO_); }
        if (billboardProgram_ != 0) { glDeleteProgram(billboardProgram_); }
        if (billboardVAO_ != 0) { glDeleteVertexArrays(1, &billboardVAO_); }
        if (billboardVBO_ != 0) { glDeleteBuffers(1, &billboardVBO_); }
    }

    void ImposterSystem::init(const Config& config) {
        config_ = config;

        // Create FBO for capturing imposter views
        glCreateFramebuffers(1, &captureFBO_);
        glCreateRenderbuffers(1, &captureRBO_);

        int viewSize = config_.atlasSize / config_.viewsPerRow;
        glNamedRenderbufferStorage(captureRBO_, GL_DEPTH_COMPONENT24, viewSize, viewSize);
        glNamedFramebufferRenderbuffer(captureFBO_, GL_DEPTH_ATTACHMENT,
                                       GL_RENDERBUFFER, captureRBO_);

        // Compile billboard shader
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
                NOX_LOG_ERROR("Imposter shader error: {}", log);
                glDeleteShader(shader);
                return 0;
            }
            return shader;
        };

        GLuint vs = compileShader(GL_VERTEX_SHADER, ImposterVertexSource);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, ImposterFragmentSource);
        if (vs != 0 && fs != 0) {
            billboardProgram_ = glCreateProgram();
            glAttachShader(billboardProgram_, vs);
            glAttachShader(billboardProgram_, fs);
            glLinkProgram(billboardProgram_);
        }
        if (vs) { glDeleteShader(vs); }
        if (fs) { glDeleteShader(fs); }

        // Create billboard quad VAO
        // Two triangles forming a unit quad centered at origin
        float quadVerts[] = {
            // position (xy), UV
            -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.0f, 1.0f,
        };

        glCreateVertexArrays(1, &billboardVAO_);
        glCreateBuffers(1, &billboardVBO_);
        glNamedBufferStorage(billboardVBO_, sizeof(quadVerts), quadVerts, 0);

        glVertexArrayVertexBuffer(billboardVAO_, 0, billboardVBO_, 0, 4 * sizeof(float));
        glEnableVertexArrayAttrib(billboardVAO_, 0);
        glVertexArrayAttribFormat(billboardVAO_, 0, 2, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(billboardVAO_, 0, 0);

        glEnableVertexArrayAttrib(billboardVAO_, 1);
        glVertexArrayAttribFormat(billboardVAO_, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float));
        glVertexArrayAttribBinding(billboardVAO_, 1, 0);

        initialized_ = true;
        NOX_LOG_INFO("ImposterSystem initialized (atlas {}x{}, {}x{} views)",
                     config_.atlasSize, config_.atlasSize,
                     config_.viewsPerRow, config_.viewRows);
    }

    uint32_t ImposterSystem::generateAtlas(
            [[maybe_unused]] const std::shared_ptr<Mesh>& mesh) {
        if (!initialized_) { return 0; }

        int viewSize = config_.atlasSize / config_.viewsPerRow;

        // Create atlas texture
        uint32_t atlas = 0;
        glCreateTextures(GL_TEXTURE_2D, 1, &atlas);
        glTextureStorage2D(atlas, 1, GL_RGBA8, config_.atlasSize, config_.atlasSize);
        glTextureParameteri(atlas, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(atlas, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glNamedFramebufferTexture(captureFBO_, GL_COLOR_ATTACHMENT0, atlas, 0);

        // Render mesh from multiple angles into atlas
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO_);

        for (int row = 0; row < config_.viewRows; ++row) {
            float elevation = -30.0f + 60.0f * static_cast<float>(row)
                              / static_cast<float>(config_.viewRows - 1);

            for (int col = 0; col < config_.viewsPerRow; ++col) {
                float azimuth = 360.0f * static_cast<float>(col)
                                / static_cast<float>(config_.viewsPerRow);

                int vpX = col * viewSize;
                int vpY = row * viewSize;
                glViewport(vpX, vpY, viewSize, viewSize);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // Setup camera looking at mesh from (azimuth, elevation)
                float radAz = glm::radians(azimuth);
                float radEl = glm::radians(elevation);
                float dist = 2.0f;

                Math::Vec3 eye(
                    dist * std::cos(radEl) * std::cos(radAz),
                    dist * std::sin(radEl),
                    dist * std::cos(radEl) * std::sin(radAz)
                );

                [[maybe_unused]] Math::Mat4 view = glm::lookAt(eye, Math::Vec3(0.0f), Math::Vec3(0.0f, 1.0f, 0.0f));
                [[maybe_unused]] Math::Mat4 proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

                // In full implementation, render the mesh here with view/proj
                // mesh->render(view, proj);
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        NOX_LOG_INFO("Imposter atlas generated ({}x{} views)", config_.viewsPerRow, config_.viewRows);
        return atlas;
    }

    void ImposterSystem::render(const std::vector<ImposterData>& imposters,
                                 const Math::Mat4& viewMatrix,
                                 const Math::Mat4& projMatrix,
                                 const Math::Vec3& cameraPos) {
        if (!initialized_ || billboardProgram_ == 0 || imposters.empty()) { return; }

        glUseProgram(billboardProgram_);

        Math::Mat4 viewProj = projMatrix * viewMatrix;
        glUniformMatrix4fv(glGetUniformLocation(billboardProgram_, "uViewProj"),
                           1, GL_FALSE, glm::value_ptr(viewProj));

        // Extract camera right and up vectors from view matrix
        Math::Vec3 camRight(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
        Math::Vec3 camUp(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
        glUniform3fv(glGetUniformLocation(billboardProgram_, "uCamRight"),
                     1, glm::value_ptr(camRight));
        glUniform3fv(glGetUniformLocation(billboardProgram_, "uCamUp"),
                     1, glm::value_ptr(camUp));

        glBindVertexArray(billboardVAO_);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float uvW = 1.0f / static_cast<float>(config_.viewsPerRow);
        float uvH = 1.0f / static_cast<float>(config_.viewRows);

        for (const auto& imp : imposters) {
            float dist = glm::length(imp.position - cameraPos);
            if (dist < config_.switchDistance) { continue; }

            glUniform3fv(glGetUniformLocation(billboardProgram_, "uBillboardPos"),
                         1, glm::value_ptr(imp.position));
            glUniform1f(glGetUniformLocation(billboardProgram_, "uBillboardSize"),
                        imp.size);

            // Select atlas view based on camera angle
            Math::Vec3 toCamera = glm::normalize(cameraPos - imp.position);
            float azimuth = std::atan2(toCamera.z, toCamera.x);
            if (azimuth < 0.0f) { azimuth += glm::two_pi<float>(); }
            int col = static_cast<int>(azimuth / glm::two_pi<float>()
                                       * static_cast<float>(config_.viewsPerRow))
                      % config_.viewsPerRow;
            int row = config_.viewRows / 2; // Simplified: always use middle elevation

            glUniform4f(glGetUniformLocation(billboardProgram_, "uAtlasRect"),
                        static_cast<float>(col) * uvW,
                        static_cast<float>(row) * uvH,
                        uvW, uvH);

            glBindTextureUnit(0, imp.textureId);
            glUniform1i(glGetUniformLocation(billboardProgram_, "uAtlas"), 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glDisable(GL_BLEND);
    }

} // namespace Nox

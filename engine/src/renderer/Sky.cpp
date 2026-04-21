// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/Sky.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <vector>

namespace Nox {

    // ── Preetham sky model shaders with Rayleigh + Mie scattering ─

    static constexpr std::string_view SkyVertexSource = R"glsl(
    #version 460 core
    layout(location = 0) in vec3 aPosition;

    uniform mat4 uView;
    uniform mat4 uProjection;

    out vec3 vWorldPos;

    void main() {
        vWorldPos = aPosition;
        // Remove translation from view matrix (sky dome follows camera)
        mat4 rotView = mat4(mat3(uView));
        vec4 clipPos = uProjection * rotView * vec4(aPosition, 1.0);
        gl_Position = clipPos.xyww; // depth = 1.0 (behind everything)
    }
    )glsl";

    static constexpr std::string_view SkyFragmentSource = R"glsl(
    #version 460 core
    in vec3 vWorldPos;

    uniform vec3  uSunDirection;
    uniform float uTurbidity;
    uniform float uSunIntensity;
    uniform float uRayleighCoeff;
    uniform float uMieCoeff;
    uniform float uMieG;

    out vec4 FragColor;

    const float PI = 3.14159265359;
    const vec3 lambda = vec3(680e-9, 550e-9, 450e-9); // RGB wavelengths

    // Rayleigh scattering phase function
    float rayleighPhase(float cosTheta) {
        return (3.0 / (16.0 * PI)) * (1.0 + cosTheta * cosTheta);
    }

    // Mie scattering phase function (Henyey-Greenstein)
    float miePhase(float cosTheta, float g) {
        float g2 = g * g;
        float num = (1.0 - g2);
        float denom = pow(1.0 + g2 - 2.0 * g * cosTheta, 1.5);
        return (1.0 / (4.0 * PI)) * num / denom;
    }

    void main() {
        vec3 direction = normalize(vWorldPos);

        // Sun angle
        float cosTheta = dot(direction, normalize(uSunDirection));
        float sunAngle = max(dot(normalize(uSunDirection), vec3(0, 1, 0)), 0.0);

        // Optical depth approximation
        float zenithAngle = acos(max(direction.y, 0.001));
        float opticalDepth = 1.0 / (cos(zenithAngle) + 0.15 * pow(93.885 - degrees(zenithAngle), -1.253));

        // Rayleigh scattering coefficients (wavelength-dependent)
        vec3 rayleighBeta = uRayleighCoeff * vec3(5.8e-6, 13.5e-6, 33.1e-6);

        // Mie scattering (wavelength-independent)
        float mieBeta = uMieCoeff * uTurbidity;

        // Total extinction
        vec3 extinction = exp(-(rayleighBeta + mieBeta) * opticalDepth);

        // Scattering
        vec3 rayleighScatter = rayleighBeta * rayleighPhase(cosTheta);
        float mieScatter = mieBeta * miePhase(cosTheta, uMieG);

        vec3 totalScatter = (rayleighScatter + mieScatter) * uSunIntensity;
        vec3 color = totalScatter * (1.0 - extinction);

        // Sun disc
        float sunDisc = smoothstep(0.9995, 0.9998, cosTheta) * uSunIntensity * 2.0;
        color += vec3(sunDisc) * vec3(1.0, 0.9, 0.7);

        // Horizon brightening based on sun height
        float horizon = 1.0 - max(direction.y, 0.0);
        horizon = pow(horizon, 4.0) * 0.3 * sunAngle;
        color += vec3(1.0, 0.7, 0.4) * horizon;

        // Ground (below horizon, dark)
        if (direction.y < 0.0) {
            float t = clamp(-direction.y * 10.0, 0.0, 1.0);
            color = mix(color, vec3(0.05, 0.05, 0.08), t);
        }

        FragColor = vec4(color, 1.0);
    }
    )glsl";

    // ── Implementation ─────────────────────────────────────────────

    Sky::Sky() = default;

    Sky::~Sky() {
        if (initialized_) {
            glDeleteVertexArrays(1, &skyVAO_);
            glDeleteBuffers(1, &skyVBO_);
            glDeleteBuffers(1, &skyIBO_);
            if (skyProgram_ != 0) {
                glDeleteProgram(skyProgram_);
            }
        }
    }

    void Sky::setSunDirection(const Math::Vec3& dir) { sunDirection_ = glm::normalize(dir); }
    void Sky::setTurbidity(float t) { turbidity_ = glm::clamp(t, 1.0f, 20.0f); }
    void Sky::setGroundAlbedo(float a) { groundAlbedo_ = glm::clamp(a, 0.0f, 1.0f); }
    void Sky::setSunIntensity(float i) { sunIntensity_ = i; }
    void Sky::setRayleighCoefficient(float c) { rayleighCoefficient_ = c; }
    void Sky::setMieCoefficient(float c) { mieCoefficient_ = c; }
    void Sky::setMieDirectionalG(float g) { mieDirectionalG_ = glm::clamp(g, -0.999f, 0.999f); }

    void Sky::init() {
        if (initialized_) { return; }
        compileSkyShader();
        createSkyDome();
        initialized_ = true;
    }

    void Sky::compileSkyShader() {
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
                NOX_LOG_ERROR("Sky shader compile error: {}", log);
                glDeleteShader(shader);
                return 0;
            }
            return shader;
        };

        GLuint vs = compileShader(GL_VERTEX_SHADER, SkyVertexSource);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, SkyFragmentSource);

        if (vs == 0 || fs == 0) {
            if (vs) { glDeleteShader(vs); }
            if (fs) { glDeleteShader(fs); }
            return;
        }

        skyProgram_ = glCreateProgram();
        glAttachShader(skyProgram_, vs);
        glAttachShader(skyProgram_, fs);
        glLinkProgram(skyProgram_);

        GLint linked = 0;
        glGetProgramiv(skyProgram_, GL_LINK_STATUS, &linked);
        if (!linked) {
            char log[512]{};
            glGetProgramInfoLog(skyProgram_, sizeof(log), nullptr, log);
            NOX_LOG_ERROR("Sky shader link error: {}", log);
            glDeleteProgram(skyProgram_);
            skyProgram_ = 0;
        }

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    void Sky::createSkyDome() {
        // Create an inverted sphere (normals facing inward)
        constexpr int Stacks = 32;
        constexpr int Slices = 64;
        constexpr float Radius = 500.0f;

        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        for (int i = 0; i <= Stacks; ++i) {
            float phi = glm::pi<float>() * static_cast<float>(i) / static_cast<float>(Stacks);
            for (int j = 0; j <= Slices; ++j) {
                float theta = 2.0f * glm::pi<float>() * static_cast<float>(j) / static_cast<float>(Slices);

                float x = Radius * std::sin(phi) * std::cos(theta);
                float y = Radius * std::cos(phi);
                float z = Radius * std::sin(phi) * std::sin(theta);

                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            }
        }

        for (int i = 0; i < Stacks; ++i) {
            for (int j = 0; j < Slices; ++j) {
                uint32_t a = static_cast<uint32_t>(i * (Slices + 1) + j);
                uint32_t b = a + static_cast<uint32_t>(Slices + 1);

                // Inverted winding for inside-out rendering
                indices.push_back(a);
                indices.push_back(a + 1);
                indices.push_back(b);

                indices.push_back(b);
                indices.push_back(a + 1);
                indices.push_back(b + 1);
            }
        }

        skyIndexCount_ = static_cast<uint32_t>(indices.size());

        glCreateVertexArrays(1, &skyVAO_);
        glCreateBuffers(1, &skyVBO_);
        glCreateBuffers(1, &skyIBO_);

        glNamedBufferStorage(skyVBO_,
            static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
            vertices.data(), 0);

        glNamedBufferStorage(skyIBO_,
            static_cast<GLsizeiptr>(indices.size() * sizeof(uint32_t)),
            indices.data(), 0);

        glVertexArrayVertexBuffer(skyVAO_, 0, skyVBO_, 0, 3 * sizeof(float));
        glVertexArrayElementBuffer(skyVAO_, skyIBO_);

        glEnableVertexArrayAttrib(skyVAO_, 0);
        glVertexArrayAttribFormat(skyVAO_, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(skyVAO_, 0, 0);
    }

    void Sky::render(const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix,
                      [[maybe_unused]] const Math::Vec3& cameraPos) {
        if (!initialized_ || skyProgram_ == 0) { return; }

        // Draw sky behind everything
        glDepthFunc(GL_LEQUAL);
        glUseProgram(skyProgram_);

        glUniformMatrix4fv(glGetUniformLocation(skyProgram_, "uView"),
                           1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(skyProgram_, "uProjection"),
                           1, GL_FALSE, glm::value_ptr(projMatrix));
        glUniform3fv(glGetUniformLocation(skyProgram_, "uSunDirection"),
                     1, glm::value_ptr(sunDirection_));
        glUniform1f(glGetUniformLocation(skyProgram_, "uTurbidity"), turbidity_);
        glUniform1f(glGetUniformLocation(skyProgram_, "uSunIntensity"), sunIntensity_);
        glUniform1f(glGetUniformLocation(skyProgram_, "uRayleighCoeff"), rayleighCoefficient_);
        glUniform1f(glGetUniformLocation(skyProgram_, "uMieCoeff"), mieCoefficient_);
        glUniform1f(glGetUniformLocation(skyProgram_, "uMieG"), mieDirectionalG_);

        glBindVertexArray(skyVAO_);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(skyIndexCount_), GL_UNSIGNED_INT, nullptr);

        glDepthFunc(GL_LESS);
    }

} // namespace Nox

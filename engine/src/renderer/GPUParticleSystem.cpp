// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/GPUParticleSystem.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>

namespace Nox {

    // ── Compute shader for particle simulation ─────────────────────
    static constexpr std::string_view ParticleComputeSource = R"glsl(
    #version 460 core

    layout(local_size_x = 256) in;

    struct Particle {
        vec4 position;   // xyz = position, w = size
        vec4 velocity;   // xyz = velocity, w = life
        vec4 colorStart;
        vec4 colorEnd;
        float lifeMax;
        float padding1;
        float padding2;
        float padding3;
    };

    layout(std430, binding = 0) buffer ParticleBuffer {
        Particle particles[];
    };

    layout(std430, binding = 1) buffer CounterBuffer {
        int activeCount;
    };

    uniform float uDeltaTime;
    uniform float uTime;
    uniform vec3  uGravity;
    uniform int   uMaxParticles;

    void main() {
        uint idx = gl_GlobalInvocationID.x;
        if (idx >= uint(uMaxParticles)) return;

        Particle p = particles[idx];

        // Update life
        p.velocity.w -= uDeltaTime;

        if (p.velocity.w > 0.0) {
            // Apply gravity
            p.velocity.xyz += uGravity * uDeltaTime;

            // Integrate position
            p.position.xyz += p.velocity.xyz * uDeltaTime;

            // Interpolate size
            float t = 1.0 - (p.velocity.w / p.lifeMax);
            p.position.w = mix(p.colorStart.w, p.colorEnd.w, t);
        }

        particles[idx] = p;
    }
    )glsl";

    // ── Vertex shader for particle rendering ───────────────────────
    static constexpr std::string_view ParticleVertexSource = R"glsl(
    #version 460 core

    struct Particle {
        vec4 position;
        vec4 velocity;
        vec4 colorStart;
        vec4 colorEnd;
        float lifeMax;
        float padding1;
        float padding2;
        float padding3;
    };

    layout(std430, binding = 0) readonly buffer ParticleBuffer {
        Particle particles[];
    };

    uniform mat4 uView;
    uniform mat4 uProjection;
    uniform int  uMaxParticles;

    out vec4 vColor;
    out float vLife;

    void main() {
        int idx = gl_VertexID;
        if (idx >= uMaxParticles) {
            gl_Position = vec4(0.0);
            return;
        }

        Particle p = particles[idx];

        if (p.velocity.w <= 0.0) {
            gl_Position = vec4(0.0);
            gl_PointSize = 0.0;
            return;
        }

        float t = 1.0 - (p.velocity.w / p.lifeMax);
        vColor = mix(p.colorStart, p.colorEnd, t);
        vLife = p.velocity.w;

        vec4 viewPos = uView * vec4(p.position.xyz, 1.0);
        gl_Position = uProjection * viewPos;
        gl_PointSize = max(p.position.w * 100.0 / max(-viewPos.z, 0.1), 1.0);
    }
    )glsl";

    // ── Fragment shader for particle rendering ─────────────────────
    static constexpr std::string_view ParticleFragmentSource = R"glsl(
    #version 460 core

    in vec4 vColor;
    in float vLife;

    out vec4 FragColor;

    void main() {
        // Soft circle
        vec2 coord = gl_PointCoord * 2.0 - 1.0;
        float dist = dot(coord, coord);
        if (dist > 1.0) discard;

        float alpha = vColor.a * (1.0 - dist);
        FragColor = vec4(vColor.rgb, alpha);
    }
    )glsl";

    GPUParticleSystem::GPUParticleSystem()
        : SceneObject("GPUParticles", SceneObjectType::Unknown)
    {}

    GPUParticleSystem::~GPUParticleSystem() {
        destroyGPUResources();
    }

    void GPUParticleSystem::init(const Config& config) {
        config_ = config;
        destroyGPUResources();
        createGPUResources();
        initialized_ = true;
        NOX_LOG_INFO("GPU Particle System initialized ({} max particles)", config_.maxParticles);
    }

    void GPUParticleSystem::createGPUResources() {
        // Compile compute shader
        if (!simulateShader_.compile(ParticleComputeSource)) {
            NOX_LOG_ERROR("Failed to compile particle compute shader");
            return;
        }

        // Compile render program
        {
            GLuint vs = glCreateShader(GL_VERTEX_SHADER);
            const char* vsSrc = ParticleVertexSource.data();
            auto vsLen = static_cast<GLint>(ParticleVertexSource.size());
            glShaderSource(vs, 1, &vsSrc, &vsLen);
            glCompileShader(vs);

            GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
            const char* fsSrc = ParticleFragmentSource.data();
            auto fsLen = static_cast<GLint>(ParticleFragmentSource.size());
            glShaderSource(fs, 1, &fsSrc, &fsLen);
            glCompileShader(fs);

            renderProgram_ = glCreateProgram();
            glAttachShader(renderProgram_, vs);
            glAttachShader(renderProgram_, fs);
            glLinkProgram(renderProgram_);
            glDeleteShader(vs);
            glDeleteShader(fs);
        }

        // Particle buffer (SSBO)
        struct ParticleGPU {
            Math::Vec4 position;
            Math::Vec4 velocity;
            Math::Vec4 colorStart;
            Math::Vec4 colorEnd;
            float lifeMax;
            float padding1;
            float padding2;
            float padding3;
        };

        auto bufferSize = static_cast<GLsizeiptr>(config_.maxParticles * sizeof(ParticleGPU));
        glCreateBuffers(1, &particleSSBO_);
        glNamedBufferStorage(particleSSBO_, bufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);

        // Initialize all particles to dead (life <= 0)
        std::vector<ParticleGPU> initData(static_cast<size_t>(config_.maxParticles), ParticleGPU{});
        glNamedBufferSubData(particleSSBO_, 0, bufferSize, initData.data());

        // Counter buffer
        glCreateBuffers(1, &counterBuffer_);
        int zero = 0;
        glNamedBufferStorage(counterBuffer_, sizeof(int), &zero, GL_DYNAMIC_STORAGE_BIT);

        // Empty VAO for point rendering
        glCreateVertexArrays(1, &renderVAO_);
    }

    void GPUParticleSystem::destroyGPUResources() {
        if (particleSSBO_ != 0)  { glDeleteBuffers(1, &particleSSBO_); particleSSBO_ = 0; }
        if (counterBuffer_ != 0) { glDeleteBuffers(1, &counterBuffer_); counterBuffer_ = 0; }
        if (renderVAO_ != 0)     { glDeleteVertexArrays(1, &renderVAO_); renderVAO_ = 0; }
        if (renderProgram_ != 0) { glDeleteProgram(renderProgram_); renderProgram_ = 0; }
        initialized_ = false;
    }

    void GPUParticleSystem::update(float dt) {
        if (!initialized_) { return; }

        time_ += dt;

        // Emit new particles by resurrecting dead ones
        emitAccumulator_ += config_.emitRate * dt;
        int toEmit = static_cast<int>(emitAccumulator_);
        emitAccumulator_ -= static_cast<float>(toEmit);

        if (toEmit > 0) {
            // Map the buffer to find dead particles and resurrect them
            struct ParticleGPU {
                Math::Vec4 position;
                Math::Vec4 velocity;
                Math::Vec4 colorStart;
                Math::Vec4 colorEnd;
                float lifeMax;
                float padding1;
                float padding2;
                float padding3;
            };

            auto bufSize = static_cast<GLsizeiptr>(config_.maxParticles * sizeof(ParticleGPU));
            auto* particles = static_cast<ParticleGPU*>(
                glMapNamedBufferRange(particleSSBO_, 0, bufSize, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT));

            if (particles) {
                int emitted = 0;
                for (int i = 0; i < config_.maxParticles && emitted < toEmit; ++i) {
                    if (particles[i].velocity.w <= 0.0f) {
                        // Resurrect this particle
                        auto& p = particles[i];
                        Math::Vec3 pos = transform().position();
                        p.position = Math::Vec4(pos, config_.sizeStart);

                        // Random direction within emit cone
                        float angle1 = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 6.2831853f;
                        float angle2 = (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * config_.emitSpread;
                        float sinA2 = std::sin(angle2);
                        Math::Vec3 dir = glm::normalize(config_.emitDirection +
                            Math::Vec3(sinA2 * std::cos(angle1), sinA2 * std::sin(angle1), 0.0f) * config_.emitSpread);

                        float speed = config_.speedMin +
                            (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * (config_.speedMax - config_.speedMin);
                        float life = config_.lifeMin +
                            (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * (config_.lifeMax - config_.lifeMin);

                        p.velocity = Math::Vec4(dir * speed, life);
                        p.colorStart = config_.colorStart;
                        p.colorEnd = config_.colorEnd;
                        p.lifeMax = life;
                        ++emitted;
                    }
                }
                glUnmapNamedBuffer(particleSSBO_);
            }
        }

        // Dispatch compute shader for simulation
        ComputeShader::bindSSBO(particleSSBO_, 0);
        ComputeShader::bindSSBO(counterBuffer_, 1);

        simulateShader_.bind();
        simulateShader_.setUniform("uDeltaTime", dt);
        simulateShader_.setUniform("uTime", time_);
        simulateShader_.setUniform("uGravity", config_.gravity);
        simulateShader_.setUniform("uMaxParticles", config_.maxParticles);

        uint32_t groups = (static_cast<uint32_t>(config_.maxParticles) + 255) / 256;
        simulateShader_.dispatch(groups);
        ComputeShader::barrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    }

    void GPUParticleSystem::render(const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix) {
        if (!initialized_) { return; }

        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);

        glUseProgram(renderProgram_);
        ComputeShader::bindSSBO(particleSSBO_, 0);

        glUniformMatrix4fv(glGetUniformLocation(renderProgram_, "uView"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(renderProgram_, "uProjection"), 1, GL_FALSE, glm::value_ptr(projMatrix));
        glUniform1i(glGetUniformLocation(renderProgram_, "uMaxParticles"), config_.maxParticles);

        glBindVertexArray(renderVAO_);
        glDrawArrays(GL_POINTS, 0, config_.maxParticles);

        glDepthMask(GL_TRUE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_BLEND);
        glDisable(GL_PROGRAM_POINT_SIZE);
    }

} // namespace Nox

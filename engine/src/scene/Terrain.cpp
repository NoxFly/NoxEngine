// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/scene/Terrain.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef NOX_HAS_STB_IMAGE
#include <stb_image.h>
#endif

#include <algorithm>
#include <cmath>
#include <numeric>

namespace Nox {

    // ── Terrain shaders ────────────────────────────────────────────

    static constexpr std::string_view TerrainVertexSource = R"glsl(
    #version 460 core

    layout(location = 0) in vec3 aPosition;
    layout(location = 1) in vec3 aNormal;
    layout(location = 2) in vec2 aUV;

    uniform mat4 uViewProj;

    out vec3 vWorldPos;
    out vec3 vNormal;
    out vec2 vUV;

    void main() {
        vWorldPos = aPosition;
        vNormal = aNormal;
        vUV = aUV;
        gl_Position = uViewProj * vec4(aPosition, 1.0);
    }
    )glsl";

    static constexpr std::string_view TerrainFragmentSource = R"glsl(
    #version 460 core

    in vec3 vWorldPos;
    in vec3 vNormal;
    in vec2 vUV;

    uniform sampler2D uSplatMap;
    uniform sampler2D uLayer0;
    uniform sampler2D uLayer1;
    uniform sampler2D uLayer2;
    uniform sampler2D uLayer3;
    uniform vec3 uLightDir;
    uniform vec3 uLightColor;
    uniform float uAmbient;

    out vec4 FragColor;

    void main() {
        // Sample splat weights
        vec4 splat = texture(uSplatMap, vUV);

        // Detail UV (tiled)
        vec2 detailUV = vWorldPos.xz * 0.1;

        // Blend terrain layers
        vec3 color = texture(uLayer0, detailUV).rgb * splat.r
                   + texture(uLayer1, detailUV).rgb * splat.g
                   + texture(uLayer2, detailUV).rgb * splat.b
                   + texture(uLayer3, detailUV).rgb * splat.a;

        // Normalize weights (in case they don't sum to 1)
        float totalWeight = splat.r + splat.g + splat.b + splat.a;
        if (totalWeight > 0.001) {
            color /= totalWeight;
        }

        // Simple directional lighting
        vec3 N = normalize(vNormal);
        vec3 L = normalize(-uLightDir);
        float NdotL = max(dot(N, L), 0.0);
        vec3 lighting = uLightColor * NdotL + vec3(uAmbient);

        FragColor = vec4(color * lighting, 1.0);
    }
    )glsl";

    Terrain::~Terrain() {
        for (auto& chunk : chunks_) {
            destroyChunk(chunk);
        }
        if (terrainProgram_ != 0) {
            glDeleteProgram(terrainProgram_);
        }
    }

    void Terrain::init(const Config& config) {
        config_ = config;

        int totalChunks = config_.numChunksX * config_.numChunksZ;
        chunks_.resize(totalChunks);

        for (int z = 0; z < config_.numChunksZ; ++z) {
            for (int x = 0; x < config_.numChunksX; ++x) {
                int idx = z * config_.numChunksX + x;
                chunks_[idx].gridX = x;
                chunks_[idx].gridZ = z;
            }
        }

        compilePipeline();
        initialized_ = true;

        NOX_LOG_INFO("Terrain initialized ({}x{} chunks, chunk size {})",
                     config_.numChunksX, config_.numChunksZ, config_.chunkSize);
    }

    void Terrain::loadHeightmap(const std::string& path) {
#ifdef NOX_HAS_STB_IMAGE
        int w = 0;
        int h = 0;
        int channels = 0;
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 1);
        if (!data) {
            NOX_LOG_ERROR("Failed to load heightmap: {}", path);
            return;
        }

        heightmapWidth_ = w;
        heightmapHeight_ = h;
        globalHeightmap_.resize(static_cast<size_t>(w * h));

        for (int i = 0; i < w * h; ++i) {
            globalHeightmap_[i] = static_cast<float>(data[i]) / 255.0f * config_.heightScale;
        }

        stbi_image_free(data);

        // Build all chunks
        for (auto& chunk : chunks_) {
            buildChunkMesh(chunk, 0);
            uploadChunk(chunk);
        }

        NOX_LOG_INFO("Heightmap loaded: {}x{} from {}", w, h, path);
#else
        NOX_LOG_WARN("stb_image not available, cannot load heightmap: {}", path);
        (void)path;
#endif
    }

    void Terrain::generateFlat(float height) {
        int totalSize = config_.numChunksX * config_.chunkSize;
        heightmapWidth_ = totalSize;
        heightmapHeight_ = config_.numChunksZ * config_.chunkSize;
        globalHeightmap_.assign(
            static_cast<size_t>(heightmapWidth_ * heightmapHeight_), height);

        for (auto& chunk : chunks_) {
            buildChunkMesh(chunk, 0);
            uploadChunk(chunk);
        }
    }

    void Terrain::generateFromNoise(int seed, float frequency, int octaves) {
        int totalW = config_.numChunksX * config_.chunkSize;
        int totalH = config_.numChunksZ * config_.chunkSize;
        heightmapWidth_ = totalW;
        heightmapHeight_ = totalH;
        globalHeightmap_.resize(static_cast<size_t>(totalW * totalH));

        // Simple fractal noise (value noise)
        auto hash = [](int x, int y, int s) -> float {
            int n = x * 127 + y * 311 + s * 997;
            n = (n << 13) ^ n;
            return 1.0f - static_cast<float>((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff)
                        / 1073741824.0f;
        };

        auto smoothNoise = [&](float x, float y, int s) -> float {
            int ix = static_cast<int>(std::floor(x));
            int iy = static_cast<int>(std::floor(y));
            float fx = x - std::floor(x);
            float fy = y - std::floor(y);
            fx = fx * fx * (3.0f - 2.0f * fx); // smoothstep
            fy = fy * fy * (3.0f - 2.0f * fy);

            float v00 = hash(ix, iy, s);
            float v10 = hash(ix + 1, iy, s);
            float v01 = hash(ix, iy + 1, s);
            float v11 = hash(ix + 1, iy + 1, s);

            float i0 = v00 + (v10 - v00) * fx;
            float i1 = v01 + (v11 - v01) * fx;
            return i0 + (i1 - i0) * fy;
        };

        for (int y = 0; y < totalH; ++y) {
            for (int x = 0; x < totalW; ++x) {
                float h = 0.0f;
                float amp = 1.0f;
                float freq = frequency;
                float totalAmp = 0.0f;

                for (int o = 0; o < octaves; ++o) {
                    h += smoothNoise(static_cast<float>(x) * freq,
                                     static_cast<float>(y) * freq, seed + o) * amp;
                    totalAmp += amp;
                    amp *= 0.5f;
                    freq *= 2.0f;
                }

                h /= totalAmp;
                globalHeightmap_[static_cast<size_t>(y * totalW + x)] =
                    (h * 0.5f + 0.5f) * config_.heightScale;
            }
        }

        for (auto& chunk : chunks_) {
            buildChunkMesh(chunk, 0);
            uploadChunk(chunk);
        }

        NOX_LOG_INFO("Procedural terrain generated ({}x{}, seed={})", totalW, totalH, seed);
    }

    void Terrain::update(const Math::Vec3& cameraPosition) {
        if (!initialized_) { return; }

        for (auto& chunk : chunks_) {
            int newLod = computeLodLevel(chunk, cameraPosition);
            if (newLod != chunk.lodLevel) {
                destroyChunk(chunk);
                chunk.lodLevel = newLod;
                buildChunkMesh(chunk, newLod);
                uploadChunk(chunk);
            }
        }
    }

    void Terrain::render(const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix,
                          [[maybe_unused]] const Math::Vec3& cameraPos) {
        if (!initialized_ || terrainProgram_ == 0) { return; }

        glUseProgram(terrainProgram_);

        Math::Mat4 viewProj = projMatrix * viewMatrix;
        glUniformMatrix4fv(glGetUniformLocation(terrainProgram_, "uViewProj"),
                           1, GL_FALSE, glm::value_ptr(viewProj));

        // Default light
        glUniform3f(glGetUniformLocation(terrainProgram_, "uLightDir"),
                    -0.5f, -1.0f, -0.3f);
        glUniform3f(glGetUniformLocation(terrainProgram_, "uLightColor"),
                    1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(terrainProgram_, "uAmbient"), 0.15f);

        // Bind splat textures
        if (splatMapTex_ != 0) {
            glBindTextureUnit(0, splatMapTex_);
            glUniform1i(glGetUniformLocation(terrainProgram_, "uSplatMap"), 0);
        }
        for (int i = 0; i < 4; ++i) {
            if (splatTextures_[i] != 0) {
                glBindTextureUnit(1 + i, splatTextures_[i]);
                char name[16]{};
                std::snprintf(name, sizeof(name), "uLayer%d", i);
                glUniform1i(glGetUniformLocation(terrainProgram_, name), 1 + i);
            }
        }

        for (const auto& chunk : chunks_) {
            if (!chunk.gpuReady) { continue; }
            glBindVertexArray(chunk.vao);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(chunk.indexCount),
                           GL_UNSIGNED_INT, nullptr);
        }
    }

    void Terrain::setSplatTextures(uint32_t layer0, uint32_t layer1,
                                    uint32_t layer2, uint32_t layer3) {
        splatTextures_[0] = layer0;
        splatTextures_[1] = layer1;
        splatTextures_[2] = layer2;
        splatTextures_[3] = layer3;
    }

    void Terrain::setSplatMap(uint32_t splatMapTex) {
        splatMapTex_ = splatMapTex;
    }

    float Terrain::heightAt(float worldX, float worldZ) const {
        if (globalHeightmap_.empty()) { return 0.0f; }

        float hmX = worldX / config_.worldScale;
        float hmZ = worldZ / config_.worldScale;

        int ix = static_cast<int>(std::floor(hmX));
        int iz = static_cast<int>(std::floor(hmZ));

        ix = std::clamp(ix, 0, heightmapWidth_ - 2);
        iz = std::clamp(iz, 0, heightmapHeight_ - 2);

        float fx = hmX - std::floor(hmX);
        float fz = hmZ - std::floor(hmZ);

        float h00 = globalHeightmap_[static_cast<size_t>(iz * heightmapWidth_ + ix)];
        float h10 = globalHeightmap_[static_cast<size_t>(iz * heightmapWidth_ + ix + 1)];
        float h01 = globalHeightmap_[static_cast<size_t>((iz + 1) * heightmapWidth_ + ix)];
        float h11 = globalHeightmap_[static_cast<size_t>((iz + 1) * heightmapWidth_ + ix + 1)];

        float h0 = h00 + (h10 - h00) * fx;
        float h1 = h01 + (h11 - h01) * fx;
        return h0 + (h1 - h0) * fz;
    }

    Math::Vec3 Terrain::normalAt(float worldX, float worldZ) const {
        float eps = config_.worldScale;
        float hL = heightAt(worldX - eps, worldZ);
        float hR = heightAt(worldX + eps, worldZ);
        float hD = heightAt(worldX, worldZ - eps);
        float hU = heightAt(worldX, worldZ + eps);

        return glm::normalize(Math::Vec3(hL - hR, 2.0f * eps, hD - hU));
    }

    void Terrain::buildChunkMesh(TerrainChunk& chunk, int lod) {
        int step = 1 << lod; // LOD step: 1, 2, 4, 8...
        int chunkVerts = config_.chunkSize;
        int vertsPerSide = (chunkVerts / step) + 1;

        // Vertex data: position(3) + normal(3) + uv(2) = 8 floats per vertex
        std::vector<float> vertices;
        vertices.reserve(static_cast<size_t>(vertsPerSide * vertsPerSide * 8));

        float worldOffX = static_cast<float>(chunk.gridX * (config_.chunkSize - 1)) * config_.worldScale;
        float worldOffZ = static_cast<float>(chunk.gridZ * (config_.chunkSize - 1)) * config_.worldScale;

        Math::Vec3 minBound(std::numeric_limits<float>::max());
        Math::Vec3 maxBound(std::numeric_limits<float>::lowest());

        for (int z = 0; z < vertsPerSide; ++z) {
            for (int x = 0; x < vertsPerSide; ++x) {
                float wx = worldOffX + static_cast<float>(x * step) * config_.worldScale;
                float wz = worldOffZ + static_cast<float>(z * step) * config_.worldScale;
                float wy = heightAt(wx, wz);

                Math::Vec3 norm = normalAt(wx, wz);

                float u = static_cast<float>(x * step) / static_cast<float>(chunkVerts - 1);
                float v = static_cast<float>(z * step) / static_cast<float>(chunkVerts - 1);

                vertices.insert(vertices.end(), {
                    wx, wy, wz,
                    norm.x, norm.y, norm.z,
                    u, v
                });

                minBound = glm::min(minBound, Math::Vec3(wx, wy, wz));
                maxBound = glm::max(maxBound, Math::Vec3(wx, wy, wz));
            }
        }

        chunk.aabb = Math::AABB{ minBound, maxBound };

        // Generate indices
        std::vector<uint32_t> indices;
        indices.reserve(static_cast<size_t>((vertsPerSide - 1) * (vertsPerSide - 1) * 6));

        for (int z = 0; z < vertsPerSide - 1; ++z) {
            for (int x = 0; x < vertsPerSide - 1; ++x) {
                auto tl = static_cast<uint32_t>(z * vertsPerSide + x);
                auto tr = static_cast<uint32_t>(z * vertsPerSide + x + 1);
                auto bl = static_cast<uint32_t>((z + 1) * vertsPerSide + x);
                auto br = static_cast<uint32_t>((z + 1) * vertsPerSide + x + 1);

                indices.insert(indices.end(), { tl, bl, tr, tr, bl, br });
            }
        }

        chunk.heights = std::move(vertices);
        chunk.indexCount = static_cast<uint32_t>(indices.size());
        chunk.loaded = true;

        // Store index data temporarily in the heights vector (we'll use separate upload)
        // Actually, let's keep vertices and indices separate for upload
        chunk.heights.insert(chunk.heights.end(),
            reinterpret_cast<const float*>(indices.data()),
            reinterpret_cast<const float*>(indices.data() + indices.size()));
    }

    void Terrain::uploadChunk(TerrainChunk& chunk) {
        if (!chunk.loaded || chunk.heights.empty()) { return; }

        // Calculate vertex/index split
        size_t vertexFloats = chunk.heights.size() - chunk.indexCount;
        auto vertexBytes = static_cast<GLsizeiptr>(vertexFloats * sizeof(float));
        auto indexBytes = static_cast<GLsizeiptr>(chunk.indexCount * sizeof(uint32_t));

        glCreateVertexArrays(1, &chunk.vao);
        glCreateBuffers(1, &chunk.vbo);
        glCreateBuffers(1, &chunk.ibo);

        glNamedBufferStorage(chunk.vbo, vertexBytes, chunk.heights.data(), 0);
        glNamedBufferStorage(chunk.ibo, indexBytes,
                             chunk.heights.data() + vertexFloats, 0);

        constexpr int Stride = 8 * sizeof(float);
        glVertexArrayVertexBuffer(chunk.vao, 0, chunk.vbo, 0, Stride);
        glVertexArrayElementBuffer(chunk.vao, chunk.ibo);

        // Position
        glEnableVertexArrayAttrib(chunk.vao, 0);
        glVertexArrayAttribFormat(chunk.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(chunk.vao, 0, 0);

        // Normal
        glEnableVertexArrayAttrib(chunk.vao, 1);
        glVertexArrayAttribFormat(chunk.vao, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
        glVertexArrayAttribBinding(chunk.vao, 1, 0);

        // UV
        glEnableVertexArrayAttrib(chunk.vao, 2);
        glVertexArrayAttribFormat(chunk.vao, 2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float));
        glVertexArrayAttribBinding(chunk.vao, 2, 0);

        chunk.gpuReady = true;

        // Free CPU-side data
        chunk.heights.clear();
        chunk.heights.shrink_to_fit();
    }

    void Terrain::destroyChunk(TerrainChunk& chunk) {
        if (chunk.vao != 0) { glDeleteVertexArrays(1, &chunk.vao); chunk.vao = 0; }
        if (chunk.vbo != 0) { glDeleteBuffers(1, &chunk.vbo); chunk.vbo = 0; }
        if (chunk.ibo != 0) { glDeleteBuffers(1, &chunk.ibo); chunk.ibo = 0; }
        chunk.gpuReady = false;
        chunk.loaded = false;
    }

    void Terrain::compilePipeline() {
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
                NOX_LOG_ERROR("Terrain shader error: {}", log);
                glDeleteShader(shader);
                return 0;
            }
            return shader;
        };

        GLuint vs = compileShader(GL_VERTEX_SHADER, TerrainVertexSource);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, TerrainFragmentSource);

        if (vs == 0 || fs == 0) {
            if (vs) { glDeleteShader(vs); }
            if (fs) { glDeleteShader(fs); }
            return;
        }

        terrainProgram_ = glCreateProgram();
        glAttachShader(terrainProgram_, vs);
        glAttachShader(terrainProgram_, fs);
        glLinkProgram(terrainProgram_);
        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    int Terrain::computeLodLevel(const TerrainChunk& chunk, const Math::Vec3& camPos) const {
        Math::Vec3 chunkCenter = (chunk.aabb.min + chunk.aabb.max) * 0.5f;
        float dist = glm::length(camPos - chunkCenter);

        float baseDistance = static_cast<float>(config_.chunkSize) * config_.worldScale;

        for (int lod = 0; lod < config_.maxLodLevels; ++lod) {
            if (dist < baseDistance * config_.lodDistanceFactor * static_cast<float>(1 << lod)) {
                return lod;
            }
        }
        return config_.maxLodLevels - 1;
    }

} // namespace Nox

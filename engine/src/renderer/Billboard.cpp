// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/Billboard.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <cstdint>
#include <vector>

namespace Nox {

    // ── Billboard ──────────────────────────────────────────────────

    Billboard::Billboard(float width, float height)
        : SceneObject("Billboard")
        , width_(width)
        , height_(height) {
    }

    void Billboard::setSize(float width, float height) {
        width_  = width;
        height_ = height;
        gpuReady_ = false; // needs rebuild
    }

    void Billboard::setTexture(const std::string& path) {
        texturePath_ = path;
    }

    // ── TextRenderer ───────────────────────────────────────────────

    static constexpr std::string_view TextVertexSource = R"glsl(
    #version 460 core
    layout(location = 0) in vec2 aPosition;
    layout(location = 1) in vec2 aUV;

    uniform mat4 uProjection;

    out vec2 vUV;

    void main() {
        vUV = aUV;
        gl_Position = uProjection * vec4(aPosition, 0.0, 1.0);
    }
    )glsl";

    static constexpr std::string_view TextFragmentSource = R"glsl(
    #version 460 core
    in vec2 vUV;

    uniform sampler2D uFontAtlas;
    uniform vec3 uColor;

    out vec4 FragColor;

    void main() {
        float alpha = texture(uFontAtlas, vUV).r;
        if (alpha < 0.1) discard;
        FragColor = vec4(uColor, alpha);
    }
    )glsl";

    TextRenderer::TextRenderer() = default;

    TextRenderer::~TextRenderer() {
        if (initialized_) {
            glDeleteVertexArrays(1, &textVAO_);
            glDeleteBuffers(1, &textVBO_);
            glDeleteTextures(1, &fontAtlasTexture_);
            if (textProgram_ != 0) {
                glDeleteProgram(textProgram_);
            }
        }
    }

    void TextRenderer::init() {
        if (initialized_) { return; }

        // Compile text shader
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
                NOX_LOG_ERROR("Text shader compile error: {}", log);
                glDeleteShader(shader);
                return 0;
            }
            return shader;
        };

        GLuint vs = compileShader(GL_VERTEX_SHADER, TextVertexSource);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, TextFragmentSource);

        if (vs == 0 || fs == 0) {
            if (vs) { glDeleteShader(vs); }
            if (fs) { glDeleteShader(fs); }
            return;
        }

        textProgram_ = glCreateProgram();
        glAttachShader(textProgram_, vs);
        glAttachShader(textProgram_, fs);
        glLinkProgram(textProgram_);
        glDeleteShader(vs);
        glDeleteShader(fs);

        // Create VAO/VBO for dynamic text quads
        glCreateVertexArrays(1, &textVAO_);
        glCreateBuffers(1, &textVBO_);
        glNamedBufferStorage(textVBO_, sizeof(float) * 6 * 4 * 256,
                             nullptr, GL_DYNAMIC_STORAGE_BIT);

        glVertexArrayVertexBuffer(textVAO_, 0, textVBO_, 0, 4 * sizeof(float));

        glEnableVertexArrayAttrib(textVAO_, 0);
        glVertexArrayAttribFormat(textVAO_, 0, 2, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(textVAO_, 0, 0);

        glEnableVertexArrayAttrib(textVAO_, 1);
        glVertexArrayAttribFormat(textVAO_, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float));
        glVertexArrayAttribBinding(textVAO_, 1, 0);

        createFontAtlas();
        initialized_ = true;
    }

    void TextRenderer::createFontAtlas() {
        // Generate a minimal 8x8 pixel bitmap font for ASCII 32-126
        // Each character is stored in a 8x8 cell in a 128x64 texture (16x8 grid)
        constexpr int CellW = 8;
        constexpr int CellH = 8;
        constexpr int Cols = 16;
        constexpr int Rows = 6; // chars 32-127 = 96 chars, 16*6 = 96
        constexpr int AtlasW = Cols * CellW;  // 128
        constexpr int AtlasH = Rows * CellH;  // 48

        atlasWidth_  = static_cast<float>(AtlasW);
        atlasHeight_ = static_cast<float>(AtlasH);

        // Simple procedural font: just create filled rectangles for each character
        // A production engine would load a real font file
        std::vector<uint8_t> pixels(static_cast<size_t>(AtlasW * AtlasH), 0);

        // Fill with simple block characters (placeholder)
        for (int ch = 32; ch < 128; ++ch) {
            int idx = ch - 32;
            int col = idx % Cols;
            int row = idx / Cols;
            int baseX = col * CellW;
            int baseY = row * CellH;

            // Create simple filled rectangle for each printable char
            if (ch != ' ') {
                for (int y = 1; y < CellH - 1; ++y) {
                    for (int x = 1; x < CellW - 1; ++x) {
                        pixels[static_cast<size_t>((baseY + y) * AtlasW + baseX + x)] = 255;
                    }
                }
            }

            // Set up char info
            charMap_[ch] = CharInfo{
                .u0 = static_cast<float>(baseX) / atlasWidth_,
                .v0 = static_cast<float>(baseY) / atlasHeight_,
                .u1 = static_cast<float>(baseX + CellW) / atlasWidth_,
                .v1 = static_cast<float>(baseY + CellH) / atlasHeight_,
                .width   = static_cast<float>(CellW),
                .height  = static_cast<float>(CellH),
                .xOffset = 0.0f,
                .yOffset = 0.0f,
                .advance = static_cast<float>(CellW),
            };
        }

        // Upload to GPU
        glCreateTextures(GL_TEXTURE_2D, 1, &fontAtlasTexture_);
        glTextureStorage2D(fontAtlasTexture_, 1, GL_R8, AtlasW, AtlasH);
        glTextureSubImage2D(fontAtlasTexture_, 0, 0, 0, AtlasW, AtlasH,
                            GL_RED, GL_UNSIGNED_BYTE, pixels.data());
        glTextureParameteri(fontAtlasTexture_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(fontAtlasTexture_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    void TextRenderer::drawText(const std::string& text, float x, float y,
                                 float scale, const Math::Vec3& color) {
        drawText(text, x, y, 1280, 720, scale, color);
    }

    void TextRenderer::drawText(const std::string& text, float x, float y,
                                 int viewportWidth, int viewportHeight,
                                 float scale, const Math::Vec3& color) {
        if (!initialized_ || textProgram_ == 0) { return; }

        // Build vertex data for text quads
        std::vector<float> vertices;
        vertices.reserve(text.size() * 6 * 4);

        float curX = x;
        for (char c : text) {
            if (c < 32 || c > 127) { continue; }
            const auto& ci = charMap_[static_cast<unsigned char>(c)];

            float w = ci.width * scale;
            float h = ci.height * scale;
            float xp = curX + ci.xOffset * scale;
            float yp = y + ci.yOffset * scale;

            // Two triangles per character
            vertices.insert(vertices.end(), {
                xp, yp + h,     ci.u0, ci.v1,
                xp, yp,         ci.u0, ci.v0,
                xp + w, yp,     ci.u1, ci.v0,

                xp, yp + h,     ci.u0, ci.v1,
                xp + w, yp,     ci.u1, ci.v0,
                xp + w, yp + h, ci.u1, ci.v1,
            });

            curX += ci.advance * scale;
        }

        if (vertices.empty()) { return; }

        // Upload vertex data
        auto dataSize = static_cast<GLsizeiptr>(vertices.size() * sizeof(float));
        glNamedBufferSubData(textVBO_, 0, dataSize, vertices.data());

        // Render
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(textProgram_);

        Math::Mat4 ortho = glm::ortho(0.0f, static_cast<float>(viewportWidth),
                                       static_cast<float>(viewportHeight), 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(textProgram_, "uProjection"),
                           1, GL_FALSE, glm::value_ptr(ortho));
        glUniform3fv(glGetUniformLocation(textProgram_, "uColor"),
                     1, glm::value_ptr(color));

        glBindTextureUnit(0, fontAtlasTexture_);
        glUniform1i(glGetUniformLocation(textProgram_, "uFontAtlas"), 0);

        glBindVertexArray(textVAO_);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 4));

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

    // ── UISystem ───────────────────────────────────────────────────

    static constexpr std::string_view UIVertexSource = R"glsl(
    #version 460 core
    layout(location = 0) in vec2 aPosition;

    uniform mat4 uProjection;

    void main() {
        gl_Position = uProjection * vec4(aPosition, 0.0, 1.0);
    }
    )glsl";

    static constexpr std::string_view UIFragmentSource = R"glsl(
    #version 460 core
    uniform vec4 uColor;
    out vec4 FragColor;
    void main() {
        FragColor = uColor;
    }
    )glsl";

    UISystem::~UISystem() {
        if (initialized_) {
            glDeleteVertexArrays(1, &uiVAO_);
            glDeleteBuffers(1, &uiVBO_);
            if (uiProgram_ != 0) {
                glDeleteProgram(uiProgram_);
            }
        }
    }

    void UISystem::init() {
        if (initialized_) { return; }

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
                NOX_LOG_ERROR("UI shader compile error: {}", log);
                glDeleteShader(shader);
                return 0;
            }
            return shader;
        };

        GLuint vs = compileShader(GL_VERTEX_SHADER, UIVertexSource);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, UIFragmentSource);

        if (vs == 0 || fs == 0) {
            if (vs) { glDeleteShader(vs); }
            if (fs) { glDeleteShader(fs); }
            return;
        }

        uiProgram_ = glCreateProgram();
        glAttachShader(uiProgram_, vs);
        glAttachShader(uiProgram_, fs);
        glLinkProgram(uiProgram_);
        glDeleteShader(vs);
        glDeleteShader(fs);

        glCreateVertexArrays(1, &uiVAO_);
        glCreateBuffers(1, &uiVBO_);
        glNamedBufferStorage(uiVBO_, sizeof(float) * 12 * 128,
                             nullptr, GL_DYNAMIC_STORAGE_BIT);

        glVertexArrayVertexBuffer(uiVAO_, 0, uiVBO_, 0, 2 * sizeof(float));
        glEnableVertexArrayAttrib(uiVAO_, 0);
        glVertexArrayAttribFormat(uiVAO_, 0, 2, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(uiVAO_, 0, 0);

        initialized_ = true;
    }

    void UISystem::beginFrame() {
        pendingRects_.clear();
    }

    void UISystem::drawRect(float x, float y, float width, float height,
                             const Math::Vec4& color,
                             [[maybe_unused]] int viewportWidth,
                             [[maybe_unused]] int viewportHeight) {
        pendingRects_.push_back({ x, y, width, height, color });
    }

    void UISystem::endFrame(int viewportWidth, int viewportHeight) {
        if (!initialized_ || pendingRects_.empty()) { return; }

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(uiProgram_);

        Math::Mat4 ortho = glm::ortho(0.0f, static_cast<float>(viewportWidth),
                                       static_cast<float>(viewportHeight), 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(uiProgram_, "uProjection"),
                           1, GL_FALSE, glm::value_ptr(ortho));

        for (const auto& rect : pendingRects_) {
            float verts[12] = {
                rect.x,              rect.y + rect.height,
                rect.x,              rect.y,
                rect.x + rect.width, rect.y,
                rect.x,              rect.y + rect.height,
                rect.x + rect.width, rect.y,
                rect.x + rect.width, rect.y + rect.height,
            };

            glNamedBufferSubData(uiVBO_, 0, sizeof(verts), verts);
            glUniform4fv(glGetUniformLocation(uiProgram_, "uColor"),
                         1, glm::value_ptr(rect.color));

            glBindVertexArray(uiVAO_);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

} // namespace Nox

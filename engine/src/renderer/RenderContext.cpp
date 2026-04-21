// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/RenderContext.hpp>

#include <GL/glew.h>

namespace Nox {

    RenderContext::~RenderContext() {
        // ── Shadow resources ───────────────────────────────────────
        if (shadowFBO != 0) { glDeleteFramebuffers(1, &shadowFBO); }
        if (shadowDepthTex != 0) { glDeleteTextures(1, &shadowDepthTex); }

        for (auto& fbo : pointShadowFBOs) {
            if (fbo != 0) { glDeleteFramebuffers(1, &fbo); fbo = 0; }
        }
        for (auto& cm : pointShadowCubemaps) {
            if (cm != 0) { glDeleteTextures(1, &cm); cm = 0; }
        }

        // ── HDR resources ──────────────────────────────────────────
        if (hdrFBO != 0) { glDeleteFramebuffers(1, &hdrFBO); }
        if (hdrColorTex != 0) { glDeleteTextures(1, &hdrColorTex); }
        if (hdrDepthTex != 0) { glDeleteTextures(1, &hdrDepthTex); }
        if (screenQuadVAO != 0) { glDeleteVertexArrays(1, &screenQuadVAO); }
        if (screenQuadVBO != 0) { glDeleteBuffers(1, &screenQuadVBO); }
    }

    void RenderContext::setWireframe(bool enabled) {
        wireframe_ = enabled;
        if (enabled) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

} // namespace Nox

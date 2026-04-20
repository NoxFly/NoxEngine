// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/Renderer.hpp>

#include <algorithm>

namespace Nox {

    Renderer::Renderer(std::unique_ptr<RHI> rhi)
        : rhi_(std::move(rhi)) {
        renderQueue_.reserve(256);
    }

    Renderer::~Renderer() = default;

    void Renderer::submit(const RenderCommand& command) {
        renderQueue_.push_back(command);
    }

    void Renderer::render(const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix) {
        auto* cmd = rhi_->beginFrame();

        cmd->setViewport(0, 0, viewportWidth_, viewportHeight_);
        cmd->clear(0.1f, 0.1f, 0.12f, 1.0f, 1.0f);

        // Sort opaque front-to-back for early depth rejection
        std::sort(renderQueue_.begin(), renderQueue_.end(),
            [](const RenderCommand& a, const RenderCommand& b) {
                return a.depth < b.depth;
            });

        for (const auto& rc : renderQueue_) {
            cmd->bindPipeline(rc.pipeline);
            cmd->pushConstant("uModel", rc.worldMatrix);
            cmd->pushConstant("uView", viewMatrix);
            cmd->pushConstant("uProjection", projMatrix);
            cmd->bindVertexBuffer(rc.vbo);
            cmd->bindIndexBuffer(rc.ibo);
            cmd->drawIndexed(rc.indexCount);
        }

        rhi_->endFrame(cmd);
        renderQueue_.clear();
    }

    void Renderer::onResize(int width, int height) {
        viewportWidth_  = width;
        viewportHeight_ = height;
    }

} // namespace Nox

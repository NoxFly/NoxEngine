// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>
#include <NoxEngine/renderer/RHI.hpp>

#include <memory>
#include <vector>

namespace Nox {

    struct RenderCommand {
        PipelineHandle pipeline{};
        BufferHandle   vbo{};
        BufferHandle   ibo{};
        uint32_t       indexCount  = 0;
        Math::Mat4     worldMatrix{ 1.0f };
        float          depth       = 0.0f;
    };

    class Renderer {
    public:
        explicit Renderer(std::unique_ptr<RHI> rhi);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&&) = default;
        Renderer& operator=(Renderer&&) = default;

        void submit(const RenderCommand& command);
        void render(const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix);
        void onResize(int width, int height);

        [[nodiscard]] RHI& rhi() { return *rhi_; }
        [[nodiscard]] const RHI& rhi() const { return *rhi_; }

    private:
        std::unique_ptr<RHI>       rhi_;
        std::vector<RenderCommand> renderQueue_;
        int viewportWidth_  = 0;
        int viewportHeight_ = 0;
    };

} // namespace Nox

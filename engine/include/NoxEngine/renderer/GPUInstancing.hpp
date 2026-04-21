// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>
#include <NoxEngine/scene/SceneObject.hpp>

#include <cstdint>
#include <memory>
#include <vector>

namespace Nox {

    class Geometry;
    class Material;

    /// High-level GPU instancing: draws many copies of one mesh with a single
    /// draw call. Use scene.addInstanced(mesh, transforms) or create manually.
    class InstancedMesh : public SceneObject {
    public:
        InstancedMesh(std::shared_ptr<Geometry> geometry,
                      std::shared_ptr<Material> material,
                      std::vector<Math::Mat4> transforms = {});
        ~InstancedMesh() override;

        InstancedMesh(const InstancedMesh&) = delete;
        InstancedMesh& operator=(const InstancedMesh&) = delete;
        InstancedMesh(InstancedMesh&&) = default;
        InstancedMesh& operator=(InstancedMesh&&) = default;

        [[nodiscard]] const std::shared_ptr<Geometry>& geometry() const { return geometry_; }
        [[nodiscard]] const std::shared_ptr<Material>& material() const { return material_; }
        [[nodiscard]] const std::vector<Math::Mat4>& transforms() const { return transforms_; }
        [[nodiscard]] int instanceCount() const { return static_cast<int>(transforms_.size()); }

        /// Replace all instance transforms. Re-uploads to GPU on next render.
        void setTransforms(std::vector<Math::Mat4> transforms);

        /// Add a single instance transform. Re-uploads to GPU on next render.
        void addTransform(const Math::Mat4& transform);

        /// Clear all transforms.
        void clearTransforms();

        // ── GPU state (set by renderer, not user code) ─────────────
        [[nodiscard]] bool gpuReady() const { return gpuReady_; }
        void markGpuReady(bool ready) { gpuReady_ = ready; }
        [[nodiscard]] bool dirty() const { return dirty_; }
        void clearDirty() { dirty_ = false; }

        struct GpuData {
            uint32_t vao = 0;
            uint32_t vbo = 0;       ///< Mesh vertex buffer
            uint32_t ibo = 0;       ///< Mesh index buffer
            uint32_t instanceVBO = 0; ///< Instance transforms buffer
            uint32_t indexCount = 0;
        };

        GpuData& gpuData() { return gpuData_; }
        [[nodiscard]] const GpuData& gpuData() const { return gpuData_; }

    private:
        std::shared_ptr<Geometry> geometry_;
        std::shared_ptr<Material> material_;
        std::vector<Math::Mat4>   transforms_;
        bool gpuReady_ = false;
        bool dirty_    = true;
        GpuData gpuData_{};
    };

} // namespace Nox

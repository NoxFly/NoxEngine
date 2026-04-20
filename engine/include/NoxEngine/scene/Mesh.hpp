// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/scene/Geometry.hpp>
#include <NoxEngine/scene/Material.hpp>
#include <NoxEngine/scene/SceneObject.hpp>

#include <memory>

namespace Nox {

    class Mesh : public SceneObject {
    public:
        Mesh(std::shared_ptr<Geometry> geometry, std::shared_ptr<Material> material);
        ~Mesh() override = default;

        [[nodiscard]] const std::shared_ptr<Geometry>& geometry() const { return geometry_; }
        [[nodiscard]] const std::shared_ptr<Material>& material() const { return material_; }

        void setGeometry(std::shared_ptr<Geometry> geom) { geometry_ = std::move(geom); }
        void setMaterial(std::shared_ptr<Material> mat)  { material_ = std::move(mat); }

        [[nodiscard]] bool gpuReady() const { return gpuReady_; }
        void markGpuReady(bool ready) { gpuReady_ = ready; }

        // Internal GPU handles — set by the renderer, not by user code
        struct GpuData {
            uint32_t vao = 0;
            uint32_t vbo = 0;
            uint32_t ibo = 0;
            uint32_t indexCount = 0;
        };

        GpuData& gpuData() { return gpuData_; }
        [[nodiscard]] const GpuData& gpuData() const { return gpuData_; }

    private:
        std::shared_ptr<Geometry> geometry_;
        std::shared_ptr<Material> material_;
        bool    gpuReady_ = false;
        GpuData gpuData_{};
    };

} // namespace Nox

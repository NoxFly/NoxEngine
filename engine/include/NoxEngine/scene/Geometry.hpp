// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

#include <cstdint>
#include <memory>
#include <vector>

namespace Nox {

    struct Vertex {
        Math::Vec3 position{ 0.0f };
        Math::Vec3 normal{ 0.0f, 1.0f, 0.0f };
        Math::Vec2 uv{ 0.0f };
    };

    class Geometry {
    public:
        [[nodiscard]] static std::shared_ptr<Geometry> box(float width, float height, float depth);
        [[nodiscard]] static std::shared_ptr<Geometry> sphere(float radius, uint32_t segments, uint32_t rings);
        [[nodiscard]] static std::shared_ptr<Geometry> plane(float width, float height);
        [[nodiscard]] static std::shared_ptr<Geometry> fromVertices(
            std::vector<Vertex> vertices,
            std::vector<uint32_t> indices
        );

        [[nodiscard]] const std::vector<Vertex>&   vertices() const { return vertices_; }
        [[nodiscard]] const std::vector<uint32_t>& indices()  const { return indices_; }

    private:
        Geometry(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

        std::vector<Vertex>   vertices_;
        std::vector<uint32_t> indices_;
    };

} // namespace Nox

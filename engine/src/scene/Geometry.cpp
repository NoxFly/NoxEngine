// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/scene/Geometry.hpp>

#include <cmath>
#include <numbers>

namespace Nox {

    Geometry::Geometry(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
        : vertices_(std::move(vertices))
        , indices_(std::move(indices)) {}

    std::shared_ptr<Geometry> Geometry::box(float width, float height, float depth) {
        float hw = width  * 0.5f;
        float hh = height * 0.5f;
        float hd = depth  * 0.5f;

        std::vector<Vertex> verts;
        std::vector<uint32_t> idx;
        verts.reserve(24);
        idx.reserve(36);

        auto addFace = [&](Math::Vec3 n,
                        Math::Vec3 p0, Math::Vec3 p1,
                        Math::Vec3 p2, Math::Vec3 p3) {
            uint32_t base = static_cast<uint32_t>(verts.size());
            verts.push_back({ p0, n, { 0.0f, 0.0f } });
            verts.push_back({ p1, n, { 1.0f, 0.0f } });
            verts.push_back({ p2, n, { 1.0f, 1.0f } });
            verts.push_back({ p3, n, { 0.0f, 1.0f } });
            idx.insert(idx.end(), { base, base+1, base+2, base, base+2, base+3 });
        };

        // Front  (+Z)
        addFace({ 0, 0, 1},  {-hw,-hh, hd}, { hw,-hh, hd}, { hw, hh, hd}, {-hw, hh, hd});
        // Back   (-Z)
        addFace({ 0, 0,-1},  { hw,-hh,-hd}, {-hw,-hh,-hd}, {-hw, hh,-hd}, { hw, hh,-hd});
        // Right  (+X)
        addFace({ 1, 0, 0},  { hw,-hh, hd}, { hw,-hh,-hd}, { hw, hh,-hd}, { hw, hh, hd});
        // Left   (-X)
        addFace({-1, 0, 0},  {-hw,-hh,-hd}, {-hw,-hh, hd}, {-hw, hh, hd}, {-hw, hh,-hd});
        // Top    (+Y)
        addFace({ 0, 1, 0},  {-hw, hh, hd}, { hw, hh, hd}, { hw, hh,-hd}, {-hw, hh,-hd});
        // Bottom (-Y)
        addFace({ 0,-1, 0},  {-hw,-hh,-hd}, { hw,-hh,-hd}, { hw,-hh, hd}, {-hw,-hh, hd});

        return std::shared_ptr<Geometry>(new Geometry(std::move(verts), std::move(idx)));
    }

    std::shared_ptr<Geometry> Geometry::sphere(float radius, uint32_t segments, uint32_t rings) {
        std::vector<Vertex> verts;
        std::vector<uint32_t> idx;

        for (uint32_t y = 0; y <= rings; ++y) {
            for (uint32_t x = 0; x <= segments; ++x) {
                float xSeg = static_cast<float>(x) / static_cast<float>(segments);
                float ySeg = static_cast<float>(y) / static_cast<float>(rings);

                float theta = xSeg * 2.0f * std::numbers::pi_v<float>;
                float phi   = ySeg * std::numbers::pi_v<float>;

                Math::Vec3 pos{
                    radius * std::cos(theta) * std::sin(phi),
                    radius * std::cos(phi),
                    radius * std::sin(theta) * std::sin(phi)
                };
                Math::Vec3 normal = glm::normalize(pos);
                Math::Vec2 uv{ xSeg, ySeg };

                verts.push_back({ pos, normal, uv });
            }
        }

        for (uint32_t y = 0; y < rings; ++y) {
            for (uint32_t x = 0; x < segments; ++x) {
                uint32_t i0 = y * (segments + 1) + x;
                uint32_t i1 = i0 + 1;
                uint32_t i2 = i0 + (segments + 1);
                uint32_t i3 = i2 + 1;

                idx.insert(idx.end(), { i0, i2, i1, i1, i2, i3 });
            }
        }

        return std::shared_ptr<Geometry>(new Geometry(std::move(verts), std::move(idx)));
    }

    std::shared_ptr<Geometry> Geometry::plane(float width, float height) {
        float hw = width  * 0.5f;
        float hh = height * 0.5f;
        Math::Vec3 n{ 0.0f, 1.0f, 0.0f };

        std::vector<Vertex> verts = {
            { {-hw, 0.0f,  hh}, n, { 0.0f, 0.0f } },
            { { hw, 0.0f,  hh}, n, { 1.0f, 0.0f } },
            { { hw, 0.0f, -hh}, n, { 1.0f, 1.0f } },
            { {-hw, 0.0f, -hh}, n, { 0.0f, 1.0f } },
        };
        std::vector<uint32_t> idx = { 0, 1, 2, 0, 2, 3 };

        return std::shared_ptr<Geometry>(new Geometry(std::move(verts), std::move(idx)));
    }

    std::shared_ptr<Geometry> Geometry::fromVertices(
        std::vector<Vertex> vertices,
        std::vector<uint32_t> indices
    ) {
        return std::shared_ptr<Geometry>(new Geometry(std::move(vertices), std::move(indices)));
    }

} // namespace Nox

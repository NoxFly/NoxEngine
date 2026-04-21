// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

#include <cstdint>
#include <vector>

namespace Nox {

    class PhysicsWorld;

    /// Debug renderer that visualizes Jolt Physics colliders, contacts, and rays.
    /// Draws wireframe shapes using GL_LINES overlay.
    class PhysicsDebugRenderer {
    public:
        PhysicsDebugRenderer();
        ~PhysicsDebugRenderer();

        PhysicsDebugRenderer(const PhysicsDebugRenderer&) = delete;
        PhysicsDebugRenderer& operator=(const PhysicsDebugRenderer&) = delete;
        PhysicsDebugRenderer(PhysicsDebugRenderer&&) = default;
        PhysicsDebugRenderer& operator=(PhysicsDebugRenderer&&) = default;

        /// Initialize GPU resources for debug line rendering.
        void init();

        /// Collect debug geometry from the physics world.
        void update(const PhysicsWorld& world);

        /// Render the debug lines on top of the scene.
        void render(const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix);

        /// Enable/disable the debug renderer.
        void setEnabled(bool enabled) { enabled_ = enabled; }
        [[nodiscard]] bool isEnabled() const { return enabled_; }

        /// Enable/disable specific shape visualization.
        void setDrawColliders(bool draw) { drawColliders_ = draw; }
        void setDrawContactPoints(bool draw) { drawContacts_ = draw; }
        void setDrawAABBs(bool draw) { drawAABBs_ = draw; }

        [[nodiscard]] bool drawColliders() const { return drawColliders_; }
        [[nodiscard]] bool drawContactPoints() const { return drawContacts_; }
        [[nodiscard]] bool drawAABBs() const { return drawAABBs_; }

    private:
        struct DebugLine {
            Math::Vec3 start;
            Math::Vec3 color1;
            Math::Vec3 end;
            Math::Vec3 color2;
        };

        void addBox(const Math::Vec3& center, const Math::Vec3& halfExtents,
                    const Math::Quat& rotation, const Math::Vec3& color);
        void addSphere(const Math::Vec3& center, float radius, const Math::Vec3& color);
        void addCapsule(const Math::Vec3& center, float radius, float halfHeight,
                        const Math::Quat& rotation, const Math::Vec3& color);

        std::vector<DebugLine> lines_;
        uint32_t program_    = 0;
        uint32_t vao_        = 0;
        uint32_t vbo_        = 0;
        bool     initialized_ = false;
        bool     enabled_      = false;
        bool     drawColliders_ = true;
        bool     drawContacts_  = true;
        bool     drawAABBs_     = false;
    };

} // namespace Nox

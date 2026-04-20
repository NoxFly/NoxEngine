// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/core/Signal.hpp>
#include <NoxEngine/math/Types.hpp>
#include <NoxEngine/physics/RigidBody.hpp>

#include <memory>
#include <vector>

namespace Nox {

    struct RaycastHit {
        BodyHandle body;
        Math::Vec3 point{ 0.0f };
        Math::Vec3 normal{ 0.0f, 1.0f, 0.0f };
        float      distance = 0.0f;
    };

    struct CollisionEvent {
        BodyHandle bodyA;
        BodyHandle bodyB;
        Math::Vec3 contactPoint{ 0.0f };
        Math::Vec3 contactNormal{ 0.0f, 1.0f, 0.0f };
    };

    class PhysicsWorld {
    public:
        PhysicsWorld();
        ~PhysicsWorld();

        PhysicsWorld(const PhysicsWorld&) = delete;
        PhysicsWorld& operator=(const PhysicsWorld&) = delete;
        PhysicsWorld(PhysicsWorld&&) noexcept;
        PhysicsWorld& operator=(PhysicsWorld&&) noexcept;

        // Initialize the physics world
        void init(const Math::Vec3& gravity = Math::Vec3(0.0f, -9.81f, 0.0f));
        void shutdown();

        // Simulation step
        void update(float dt);

        // Body management
        [[nodiscard]] BodyHandle createBody(const RigidBodyDesc& desc);
        void destroyBody(BodyHandle handle);

        // Query body state
        [[nodiscard]] Math::Vec3 getPosition(BodyHandle handle) const;
        [[nodiscard]] Math::Quat getRotation(BodyHandle handle) const;
        [[nodiscard]] Math::Vec3 getLinearVelocity(BodyHandle handle) const;

        // Modify body state
        void setPosition(BodyHandle handle, const Math::Vec3& pos);
        void setRotation(BodyHandle handle, const Math::Quat& rot);
        void setLinearVelocity(BodyHandle handle, const Math::Vec3& vel);
        void setAngularVelocity(BodyHandle handle, const Math::Vec3& vel);
        void addForce(BodyHandle handle, const Math::Vec3& force);
        void addImpulse(BodyHandle handle, const Math::Vec3& impulse);

        // Raycasting
        [[nodiscard]] bool raycast(const Math::Vec3& origin, const Math::Vec3& direction,
                                   float maxDistance, RaycastHit& outHit) const;

        // Collision callbacks
        Signal<const CollisionEvent&> onCollision;

        // Configuration
        void setGravity(const Math::Vec3& g);
        [[nodiscard]] Math::Vec3 gravity() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

} // namespace Nox

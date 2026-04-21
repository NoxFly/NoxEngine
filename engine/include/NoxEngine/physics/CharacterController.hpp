// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>
#include <NoxEngine/core/Signal.hpp>

#include <cstdint>

namespace Nox {

    class PhysicsWorld;

    /// Capsule-based character controller using Jolt Physics.
    /// Handles slope limiting, step climbing, and ground detection.
    class CharacterController {
    public:
        struct Config {
            float capsuleRadius    = 0.3f;
            float capsuleHalfHeight = 0.9f;
            float maxSlopeAngle    = 45.0f;   // degrees
            float stepHeight       = 0.3f;
            float mass             = 80.0f;
            float moveSpeed        = 5.0f;
            float jumpForce        = 5.0f;
            float gravity          = -9.81f;
            Math::Vec3 startPosition{ 0.0f, 1.0f, 0.0f };
        };

        CharacterController();
        ~CharacterController();

        CharacterController(const CharacterController&) = delete;
        CharacterController& operator=(const CharacterController&) = delete;
        CharacterController(CharacterController&&) = default;
        CharacterController& operator=(CharacterController&&) = default;

        /// Initialize the controller in the given physics world.
        void init(PhysicsWorld& world, const Config& config = {});

        /// Shut down and remove the physics body.
        void shutdown();

        /// Update the character each frame.
        /// @param moveInput  Normalized 2D input (x = strafe, z = forward). Y is ignored.
        /// @param dt         Delta time in seconds.
        void update(const Math::Vec3& moveInput, float dt);

        /// Jump (if grounded).
        void jump();

        /// Teleport the character to a new position.
        void setPosition(const Math::Vec3& position);

        [[nodiscard]] Math::Vec3 position() const { return position_; }
        [[nodiscard]] Math::Vec3 velocity() const { return velocity_; }
        [[nodiscard]] bool isGrounded() const { return grounded_; }

        /// Signal emitted when the character lands after being airborne.
        Signal<> onLanded;

    private:
        PhysicsWorld* world_ = nullptr;
        uint32_t bodyId_     = 0;
        bool initialized_    = false;

        Config    config_;
        Math::Vec3 position_{ 0.0f };
        Math::Vec3 velocity_{ 0.0f };
        bool       grounded_ = false;
        bool       wasGrounded_ = false;
        float      verticalVelocity_ = 0.0f;
    };

} // namespace Nox

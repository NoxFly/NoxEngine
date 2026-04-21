// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/physics/CharacterController.hpp>
#include <NoxEngine/physics/PhysicsWorld.hpp>
#include <NoxEngine/core/Logger.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

namespace Nox {

    CharacterController::CharacterController() = default;

    CharacterController::~CharacterController() {
        shutdown();
    }

    void CharacterController::init(PhysicsWorld& world, const Config& config) {
        world_ = &world;
        config_ = config;
        position_ = config.startPosition;

        // Create a capsule rigid body for the character
        RigidBodyDesc desc{
            .shape       = CapsuleShape{ .radius = config.capsuleRadius, .halfHeight = config.capsuleHalfHeight },
            .motionType  = MotionType::Kinematic,
            .position    = config.startPosition,
            .rotation    = Math::Quat{ 1.0f, 0.0f, 0.0f, 0.0f },
            .mass        = config.mass,
            .friction    = 0.5f,
            .restitution = 0.0f,
            .linearDamping  = 0.0f,
            .angularDamping = 0.0f,
        };

        auto handle = world.createBody(desc);
        bodyId_ = handle.id;
        initialized_ = true;

        NOX_LOG_INFO("CharacterController initialized (radius={}, halfHeight={})",
                     config.capsuleRadius, config.capsuleHalfHeight);
    }

    void CharacterController::shutdown() {
        if (initialized_ && world_) {
            world_->destroyBody(BodyHandle{ bodyId_ });
            initialized_ = false;
            world_ = nullptr;
        }
    }

    void CharacterController::update(const Math::Vec3& moveInput, float dt) {
        if (!initialized_ || !world_) { return; }

        // Horizontal movement
        Math::Vec3 horizontalMove = Math::Vec3(moveInput.x, 0.0f, moveInput.z);
        if (glm::length(horizontalMove) > 0.001f) {
            horizontalMove = glm::normalize(horizontalMove) * config_.moveSpeed;
        }

        // Gravity
        if (!grounded_) {
            verticalVelocity_ += config_.gravity * dt;
        }
        else {
            verticalVelocity_ = 0.0f;
        }

        // Total velocity
        velocity_ = horizontalMove + Math::Vec3(0.0f, verticalVelocity_, 0.0f);
        position_ += velocity_ * dt;

        // Simple ground check (floor at y = 0 + capsule half height)
        float groundLevel = config_.capsuleHalfHeight + config_.capsuleRadius;
        wasGrounded_ = grounded_;

        if (position_.y <= groundLevel) {
            position_.y = groundLevel;
            verticalVelocity_ = 0.0f;
            grounded_ = true;
        }
        else {
            grounded_ = false;
        }

        // Fire landed signal
        if (grounded_ && !wasGrounded_) {
            onLanded.emit();
        }

        // Update physics body position
        world_->setPosition(BodyHandle{ bodyId_ }, position_);
    }

    void CharacterController::jump() {
        if (!initialized_ || !grounded_) { return; }

        verticalVelocity_ = config_.jumpForce;
        grounded_ = false;
    }

    void CharacterController::setPosition(const Math::Vec3& position) {
        position_ = position;
        if (initialized_ && world_) {
            world_->setPosition(BodyHandle{ bodyId_ }, position);
        }
    }

} // namespace Nox

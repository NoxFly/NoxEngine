// Copyright (c) 2026 NoxFly — AGPL-3.0

// JPH_DEBUG_RENDERER is injected via Jolt's INTERFACE compile definitions —
// no manual #define needed here.

#include <NoxEngine/physics/PhysicsWorld.hpp>
#include <NoxEngine/core/Logger.hpp>

#ifdef NOX_HAS_JOLT

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

#include <cstdarg>
#include <thread>
#include <variant>

namespace Nox {

    namespace {

        // ════════════════════════════════════════════════════════════════
        // Jolt callbacks (forward declarations)
        // ════════════════════════════════════════════════════════════════

        // Jolt validation callback
        [[maybe_unused]] static bool joltAssertFailed(const char* expression, const char* message, const char* file, unsigned int line) {
            NOX_LOG_ERROR("Jolt assert failed: {} ({}:{}) — {}", expression, file, line,
                          message ? message : "");
            return true; // break into debugger
        }

        // Jolt trace callback
        static void joltTrace([[maybe_unused]] const char* fmt, ...) {
            // Optional: forward Jolt traces to engine logger
        }

        // ════════════════════════════════════════════════════════════════
        // Jolt global manager (singleton) — ensures proper lifecycle
        // ════════════════════════════════════════════════════════════════
        class JoltManager {
        public:
            static JoltManager& instance() {
                static JoltManager inst;
                return inst;
            }

            void init() {
                if (initialized_) { return; }

                JPH::RegisterDefaultAllocator();
                JPH::Trace = joltTrace;
                JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = joltAssertFailed;)
                JPH::Factory::sInstance = new JPH::Factory();

                // Register types — bypass version check issues by directly calling with correct version
                // This helps when headers don't match the compiled library version
                NOX_LOG_INFO("Registering Jolt types...");
                JPH::RegisterTypes();

                initialized_ = true;
                NOX_LOG_INFO("JoltManager initialized");
            }

            ~JoltManager() {
                if (initialized_) {
                    delete JPH::Factory::sInstance;
                    JPH::Factory::sInstance = nullptr;
                    initialized_ = false;
                }
            }

            JoltManager(const JoltManager&) = delete;
            JoltManager& operator=(const JoltManager&) = delete;

        private:
            JoltManager() = default;
            bool initialized_ = false;
        };

        // Broad phase layers
        namespace BroadPhaseLayers {
            static constexpr JPH::BroadPhaseLayer NonMoving(0);
            static constexpr JPH::BroadPhaseLayer Moving(1);
            static constexpr uint32_t NumLayers = 2;
        }

        // Object layers
        namespace ObjectLayers {
            static constexpr JPH::ObjectLayer NonMoving = 0;
            static constexpr JPH::ObjectLayer Moving = 1;
            static constexpr uint32_t NumLayers = 2;
        }

        // BroadPhaseLayerInterface implementation
        class BPLayerInterface final : public JPH::BroadPhaseLayerInterface {
        public:
            BPLayerInterface() {
                objectToBroadPhase_[ObjectLayers::NonMoving] = BroadPhaseLayers::NonMoving;
                objectToBroadPhase_[ObjectLayers::Moving] = BroadPhaseLayers::Moving;
            }

            JPH::uint GetNumBroadPhaseLayers() const override {
                return BroadPhaseLayers::NumLayers;
            }

            JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer layer) const override {
                return objectToBroadPhase_[layer];
            }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
            const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const override {
                switch (static_cast<JPH::BroadPhaseLayer::Type>(layer)) {
                    case static_cast<JPH::BroadPhaseLayer::Type>(BroadPhaseLayers::NonMoving): return "NonMoving";
                    case static_cast<JPH::BroadPhaseLayer::Type>(BroadPhaseLayers::Moving):    return "Moving";
                    default: return "?";
                }
            }
#endif

        private:
            JPH::BroadPhaseLayer objectToBroadPhase_[ObjectLayers::NumLayers]{};
        };

        // ObjectVsBroadPhaseLayerFilter
        class ObjVsBPFilter final : public JPH::ObjectVsBroadPhaseLayerFilter {
        public:
            bool ShouldCollide(JPH::ObjectLayer obj, JPH::BroadPhaseLayer bp) const override {
                if (obj == ObjectLayers::NonMoving) {
                    return bp == BroadPhaseLayers::Moving;
                }
                return true;
            }
        };

        // ObjectLayerPairFilter
        class ObjLayerPairFilter final : public JPH::ObjectLayerPairFilter {
        public:
            bool ShouldCollide(JPH::ObjectLayer a, JPH::ObjectLayer b) const override {
                if (a == ObjectLayers::NonMoving && b == ObjectLayers::NonMoving) {
                    return false;
                }
                return true;
            }
        };

    } // anonymous namespace

    struct PhysicsWorld::Impl {
        std::unique_ptr<JPH::TempAllocatorImpl>    tempAllocator;
        std::unique_ptr<JPH::JobSystemThreadPool>  jobSystem;
        std::unique_ptr<JPH::PhysicsSystem>        physicsSystem;

        BPLayerInterface    bpLayerInterface;
        ObjVsBPFilter       objVsBPFilter;
        ObjLayerPairFilter  objLayerPairFilter;

        Math::Vec3 gravity{ 0.0f, -9.81f, 0.0f };
        bool initialized = false;

        static constexpr uint32_t MaxBodies = 4096;
        static constexpr uint32_t MaxBodyPairs = 4096;
        static constexpr uint32_t MaxContactConstraints = 2048;
        static constexpr int CollisionSteps = 1;
    };

    PhysicsWorld::PhysicsWorld()
        : impl_(std::make_unique<Impl>()) {}

    PhysicsWorld::~PhysicsWorld() {
        if (impl_ && impl_->initialized) {
            shutdown();
        }
    }

    PhysicsWorld::PhysicsWorld(PhysicsWorld&&) noexcept = default;
    PhysicsWorld& PhysicsWorld::operator=(PhysicsWorld&&) noexcept = default;

    void PhysicsWorld::init(const Math::Vec3& gravity) {
        if (impl_->initialized) { return; }

        // Initialize global Jolt manager (once)
        JoltManager::instance().init();

        impl_->gravity = gravity;

        impl_->tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);
        impl_->jobSystem = std::make_unique<JPH::JobSystemThreadPool>(
            JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers,
            static_cast<int>(std::thread::hardware_concurrency()) - 1
        );

        impl_->physicsSystem = std::make_unique<JPH::PhysicsSystem>();
        impl_->physicsSystem->Init(
            Impl::MaxBodies,
            0,
            Impl::MaxBodyPairs,
            Impl::MaxContactConstraints,
            impl_->bpLayerInterface,
            impl_->objVsBPFilter,
            impl_->objLayerPairFilter
        );

        impl_->physicsSystem->SetGravity(JPH::Vec3(gravity.x, gravity.y, gravity.z));
        impl_->initialized = true;

        NOX_LOG_INFO("PhysicsWorld initialized (Jolt Physics)");
    }

    void PhysicsWorld::shutdown() {
        if (!impl_->initialized) { return; }
        impl_->physicsSystem.reset();
        impl_->jobSystem.reset();
        impl_->tempAllocator.reset();
        impl_->initialized = false;
    }

    void PhysicsWorld::update(float dt) {
        if (!impl_->initialized) { return; }
        impl_->physicsSystem->Update(dt, Impl::CollisionSteps,
                                      impl_->tempAllocator.get(),
                                      impl_->jobSystem.get());
    }

    // ════════════════════════════════════════════════════════════════
    // Force Jolt initialization at program start via static initializer
    // This ensures JPH::RegisterTypes() is called exactly once
    // ════════════════════════════════════════════════════════════════
    namespace {
        struct JoltInitializer {
            JoltInitializer() {
                JoltManager::instance().init();
            }
        };
        static JoltInitializer joltInitializer;
    }

    BodyHandle PhysicsWorld::createBody(const RigidBodyDesc& desc) {
        if (!impl_->initialized) { return BodyHandle{}; }

        JPH::ShapeRefC shape;

        std::visit([&shape](const auto& s) {
            using T = std::decay_t<decltype(s)>;
            if constexpr (std::is_same_v<T, BoxShape>) {
                shape = new JPH::BoxShape(JPH::Vec3(s.halfExtents.x, s.halfExtents.y, s.halfExtents.z));
            }
            else if constexpr (std::is_same_v<T, SphereShape>) {
                shape = new JPH::SphereShape(s.radius);
            }
            else if constexpr (std::is_same_v<T, CapsuleShape>) {
                shape = new JPH::CapsuleShape(s.halfHeight, s.radius);
            }
        }, desc.shape);

        if (!shape) { return BodyHandle{}; }

        JPH::EMotionType motionType;
        JPH::ObjectLayer layer;

        switch (desc.motionType) {
            case MotionType::Static:
                motionType = JPH::EMotionType::Static;
                layer = ObjectLayers::NonMoving;
                break;
            case MotionType::Kinematic:
                motionType = JPH::EMotionType::Kinematic;
                layer = ObjectLayers::Moving;
                break;
            case MotionType::Dynamic:
            default:
                motionType = JPH::EMotionType::Dynamic;
                layer = ObjectLayers::Moving;
                break;
        }

        JPH::BodyCreationSettings bodySettings(
            shape,
            JPH::RVec3(desc.position.x, desc.position.y, desc.position.z),
            JPH::Quat(desc.rotation.x, desc.rotation.y, desc.rotation.z, desc.rotation.w),
            motionType,
            layer
        );

        bodySettings.mFriction = desc.friction;
        bodySettings.mRestitution = desc.restitution;
        bodySettings.mLinearDamping = desc.linearDamping;
        bodySettings.mAngularDamping = desc.angularDamping;

        auto& bodyInterface = impl_->physicsSystem->GetBodyInterface();
        JPH::BodyID bodyId = bodyInterface.CreateAndAddBody(bodySettings, JPH::EActivation::Activate);

        if (bodyId.IsInvalid()) { return BodyHandle{}; }

        return BodyHandle{ bodyId.GetIndexAndSequenceNumber() };
    }

    void PhysicsWorld::destroyBody(BodyHandle handle) {
        if (!impl_->initialized || !handle.isValid()) { return; }
        auto& bodyInterface = impl_->physicsSystem->GetBodyInterface();
        JPH::BodyID bodyId(handle.id);
        bodyInterface.RemoveBody(bodyId);
        bodyInterface.DestroyBody(bodyId);
    }

    Math::Vec3 PhysicsWorld::getPosition(BodyHandle handle) const {
        if (!impl_->initialized || !handle.isValid()) { return Math::Vec3(0.0f); }
        auto& bodyInterface = impl_->physicsSystem->GetBodyInterface();
        JPH::RVec3 pos = bodyInterface.GetCenterOfMassPosition(JPH::BodyID(handle.id));
        return Math::Vec3(static_cast<float>(pos.GetX()),
                          static_cast<float>(pos.GetY()),
                          static_cast<float>(pos.GetZ()));
    }

    Math::Quat PhysicsWorld::getRotation(BodyHandle handle) const {
        if (!impl_->initialized || !handle.isValid()) { return Math::Quat(1.0f, 0.0f, 0.0f, 0.0f); }
        auto& bodyInterface = impl_->physicsSystem->GetBodyInterface();
        JPH::Quat rot = bodyInterface.GetRotation(JPH::BodyID(handle.id));
        return Math::Quat(rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ());
    }

    Math::Vec3 PhysicsWorld::getLinearVelocity(BodyHandle handle) const {
        if (!impl_->initialized || !handle.isValid()) { return Math::Vec3(0.0f); }
        auto& bodyInterface = impl_->physicsSystem->GetBodyInterface();
        JPH::Vec3 vel = bodyInterface.GetLinearVelocity(JPH::BodyID(handle.id));
        return Math::Vec3(vel.GetX(), vel.GetY(), vel.GetZ());
    }

    void PhysicsWorld::setPosition(BodyHandle handle, const Math::Vec3& pos) {
        if (!impl_->initialized || !handle.isValid()) { return; }
        auto& bodyInterface = impl_->physicsSystem->GetBodyInterface();
        bodyInterface.SetPosition(JPH::BodyID(handle.id),
                                  JPH::RVec3(pos.x, pos.y, pos.z),
                                  JPH::EActivation::Activate);
    }

    void PhysicsWorld::setRotation(BodyHandle handle, const Math::Quat& rot) {
        if (!impl_->initialized || !handle.isValid()) { return; }
        auto& bodyInterface = impl_->physicsSystem->GetBodyInterface();
        bodyInterface.SetRotation(JPH::BodyID(handle.id),
                                  JPH::Quat(rot.x, rot.y, rot.z, rot.w),
                                  JPH::EActivation::Activate);
    }

    void PhysicsWorld::setLinearVelocity(BodyHandle handle, const Math::Vec3& vel) {
        if (!impl_->initialized || !handle.isValid()) { return; }
        auto& bodyInterface = impl_->physicsSystem->GetBodyInterface();
        bodyInterface.SetLinearVelocity(JPH::BodyID(handle.id),
                                        JPH::Vec3(vel.x, vel.y, vel.z));
    }

    void PhysicsWorld::setAngularVelocity(BodyHandle handle, const Math::Vec3& vel) {
        if (!impl_->initialized || !handle.isValid()) { return; }
        auto& bodyInterface = impl_->physicsSystem->GetBodyInterface();
        bodyInterface.SetAngularVelocity(JPH::BodyID(handle.id),
                                          JPH::Vec3(vel.x, vel.y, vel.z));
    }

    void PhysicsWorld::addForce(BodyHandle handle, const Math::Vec3& force) {
        if (!impl_->initialized || !handle.isValid()) { return; }
        auto& bodyInterface = impl_->physicsSystem->GetBodyInterface();
        bodyInterface.AddForce(JPH::BodyID(handle.id),
                               JPH::Vec3(force.x, force.y, force.z));
    }

    void PhysicsWorld::addImpulse(BodyHandle handle, const Math::Vec3& impulse) {
        if (!impl_->initialized || !handle.isValid()) { return; }
        auto& bodyInterface = impl_->physicsSystem->GetBodyInterface();
        bodyInterface.AddImpulse(JPH::BodyID(handle.id),
                                  JPH::Vec3(impulse.x, impulse.y, impulse.z));
    }

    bool PhysicsWorld::raycast(const Math::Vec3& origin, const Math::Vec3& direction,
                               float maxDistance, RaycastHit& outHit) const
    {
        if (!impl_->initialized) { return false; }

        JPH::RRayCast ray(
            JPH::RVec3(origin.x, origin.y, origin.z),
            JPH::Vec3(direction.x * maxDistance, direction.y * maxDistance, direction.z * maxDistance)
        );

        JPH::RayCastResult result;
        bool hit = impl_->physicsSystem->GetNarrowPhaseQuery().CastRay(ray, result);

        if (hit) {
            JPH::RVec3 hitPoint = ray.GetPointOnRay(result.mFraction);
            outHit.body = BodyHandle{ result.mBodyID.GetIndexAndSequenceNumber() };
            outHit.point = Math::Vec3(static_cast<float>(hitPoint.GetX()),
                                       static_cast<float>(hitPoint.GetY()),
                                       static_cast<float>(hitPoint.GetZ()));
            outHit.distance = result.mFraction * maxDistance;
        }

        return hit;
    }

    void PhysicsWorld::setGravity(const Math::Vec3& g) {
        impl_->gravity = g;
        if (impl_->initialized) {
            impl_->physicsSystem->SetGravity(JPH::Vec3(g.x, g.y, g.z));
        }
    }

    Math::Vec3 PhysicsWorld::gravity() const {
        return impl_->gravity;
    }

} // namespace Nox

#else // NOX_HAS_JOLT not defined — stub implementations

namespace Nox {

    struct PhysicsWorld::Impl {
        Math::Vec3 gravity{ 0.0f, -9.81f, 0.0f };
    };

    PhysicsWorld::PhysicsWorld() : impl_(std::make_unique<Impl>()) {}
    PhysicsWorld::~PhysicsWorld() = default;
    PhysicsWorld::PhysicsWorld(PhysicsWorld&&) noexcept = default;
    PhysicsWorld& PhysicsWorld::operator=(PhysicsWorld&&) noexcept = default;

    void PhysicsWorld::init(const Math::Vec3& g) { impl_->gravity = g; NOX_LOG_WARN("Physics disabled (no Jolt)"); }
    void PhysicsWorld::shutdown() {}
    void PhysicsWorld::update([[maybe_unused]] float dt) {}

    BodyHandle PhysicsWorld::createBody([[maybe_unused]] const RigidBodyDesc& desc) { return {}; }
    void PhysicsWorld::destroyBody([[maybe_unused]] BodyHandle h) {}

    Math::Vec3 PhysicsWorld::getPosition([[maybe_unused]] BodyHandle h) const { return {}; }
    Math::Quat PhysicsWorld::getRotation([[maybe_unused]] BodyHandle h) const { return Math::Quat(1,0,0,0); }
    Math::Vec3 PhysicsWorld::getLinearVelocity([[maybe_unused]] BodyHandle h) const { return {}; }

    void PhysicsWorld::setPosition([[maybe_unused]] BodyHandle h, [[maybe_unused]] const Math::Vec3& p) {}
    void PhysicsWorld::setRotation([[maybe_unused]] BodyHandle h, [[maybe_unused]] const Math::Quat& r) {}
    void PhysicsWorld::setLinearVelocity([[maybe_unused]] BodyHandle h, [[maybe_unused]] const Math::Vec3& v) {}
    void PhysicsWorld::setAngularVelocity([[maybe_unused]] BodyHandle h, [[maybe_unused]] const Math::Vec3& v) {}
    void PhysicsWorld::addForce([[maybe_unused]] BodyHandle h, [[maybe_unused]] const Math::Vec3& f) {}
    void PhysicsWorld::addImpulse([[maybe_unused]] BodyHandle h, [[maybe_unused]] const Math::Vec3& i) {}

    bool PhysicsWorld::raycast([[maybe_unused]] const Math::Vec3& o, [[maybe_unused]] const Math::Vec3& d,
                               [[maybe_unused]] float max, [[maybe_unused]] RaycastHit& out) const { return false; }

    void PhysicsWorld::setGravity(const Math::Vec3& g) { impl_->gravity = g; }
    Math::Vec3 PhysicsWorld::gravity() const { return impl_->gravity; }

} // namespace Nox

#endif

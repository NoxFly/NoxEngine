// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/core/Serialization.hpp>
#include <NoxEngine/math/Types.hpp>

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Nox {

    /// Unique identifier for a replicated network entity.
    using NetEntityId = uint32_t;

    /// Snapshot of a single entity's state for network replication.
    struct EntitySnapshot {
        NetEntityId entityId = 0;
        Math::Vec3 position  = { 0.0f, 0.0f, 0.0f };
        Math::Vec3 rotation  = { 0.0f, 0.0f, 0.0f }; ///< Euler degrees
        Math::Vec3 scale     = { 1.0f, 1.0f, 1.0f };
        float timestamp      = 0.0f;
    };

    /// A full snapshot of all replicated entities at a given time.
    struct WorldSnapshot {
        float timestamp = 0.0f;
        std::vector<EntitySnapshot> entities;

        void serialize(Archive& archive) const;
        void deserialize(Archive& archive);
    };

    /// Manages entity replication and snapshot interpolation.
    class EntityReplication {
    public:
        EntityReplication() = default;
        ~EntityReplication() = default;

        /// Register a new entity for replication.
        void registerEntity(NetEntityId id);

        /// Unregister an entity.
        void unregisterEntity(NetEntityId id);

        /// Update a local entity's state (will be sent to remote peers).
        void updateEntityState(NetEntityId id, const EntitySnapshot& snapshot);

        /// Take a snapshot of all registered entities.
        [[nodiscard]] WorldSnapshot takeSnapshot(float timestamp) const;

        /// Apply a received snapshot (stores for interpolation).
        void applySnapshot(const WorldSnapshot& snapshot);

        /// Interpolate between two snapshots at a given time.
        /// Returns interpolated states for all entities.
        [[nodiscard]] std::vector<EntitySnapshot> interpolate(float renderTime) const;

        /// Get the latest known state of an entity.
        [[nodiscard]] const EntitySnapshot* getEntityState(NetEntityId id) const;

        /// Set the interpolation delay (in seconds). Default: 100ms.
        void setInterpolationDelay(float delay) { interpolationDelay_ = delay; }

    private:
        struct EntityData {
            EntitySnapshot current;
            EntitySnapshot previous;
            bool registered = false;
        };

        std::unordered_map<NetEntityId, EntityData> entities_;

        // Snapshot buffer for interpolation
        static constexpr size_t MaxSnapshots = 32;
        std::vector<WorldSnapshot> snapshotBuffer_;
        float interpolationDelay_ = 0.1f;
    };

} // namespace Nox

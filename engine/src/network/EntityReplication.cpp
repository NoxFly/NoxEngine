// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/network/EntityReplication.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <algorithm>
#include <cmath>

namespace Nox {

    // ── WorldSnapshot serialization ────────────────────────────────

    void WorldSnapshot::serialize(Archive& archive) const {
        archive.write(timestamp);
        auto count = static_cast<uint32_t>(entities.size());
        archive.write(count);
        for (const auto& e : entities) {
            archive.write(e.entityId);
            archive.write(e.position.x);
            archive.write(e.position.y);
            archive.write(e.position.z);
            archive.write(e.rotation.x);
            archive.write(e.rotation.y);
            archive.write(e.rotation.z);
            archive.write(e.scale.x);
            archive.write(e.scale.y);
            archive.write(e.scale.z);
            archive.write(e.timestamp);
        }
    }

    void WorldSnapshot::deserialize(Archive& archive) {
        archive.read(timestamp);
        uint32_t count = 0;
        archive.read(count);
        entities.resize(count);
        for (uint32_t i = 0; i < count; ++i) {
            auto& e = entities[i];
            archive.read(e.entityId);
            archive.read(e.position.x);
            archive.read(e.position.y);
            archive.read(e.position.z);
            archive.read(e.rotation.x);
            archive.read(e.rotation.y);
            archive.read(e.rotation.z);
            archive.read(e.scale.x);
            archive.read(e.scale.y);
            archive.read(e.scale.z);
            archive.read(e.timestamp);
        }
    }

    // ── EntityReplication ──────────────────────────────────────────

    void EntityReplication::registerEntity(NetEntityId id) {
        entities_[id] = { {}, {}, true };
        entities_[id].current.entityId = id;
        entities_[id].previous.entityId = id;
    }

    void EntityReplication::unregisterEntity(NetEntityId id) {
        entities_.erase(id);
    }

    void EntityReplication::updateEntityState(NetEntityId id, const EntitySnapshot& snapshot) {
        auto it = entities_.find(id);
        if (it == entities_.end()) { return; }
        it->second.previous = it->second.current;
        it->second.current = snapshot;
    }

    WorldSnapshot EntityReplication::takeSnapshot(float timestamp) const {
        WorldSnapshot snap;
        snap.timestamp = timestamp;
        snap.entities.reserve(entities_.size());
        for (const auto& [id, data] : entities_) {
            if (data.registered) {
                EntitySnapshot es = data.current;
                es.timestamp = timestamp;
                snap.entities.push_back(es);
            }
        }
        return snap;
    }

    void EntityReplication::applySnapshot(const WorldSnapshot& snapshot) {
        // Store in ring buffer
        if (snapshotBuffer_.size() >= MaxSnapshots) {
            snapshotBuffer_.erase(snapshotBuffer_.begin());
        }
        snapshotBuffer_.push_back(snapshot);

        // Update entity states from snapshot
        for (const auto& es : snapshot.entities) {
            auto it = entities_.find(es.entityId);
            if (it != entities_.end()) {
                it->second.previous = it->second.current;
                it->second.current = es;
            }
        }
    }

    std::vector<EntitySnapshot> EntityReplication::interpolate(float renderTime) const {
        float targetTime = renderTime - interpolationDelay_;

        // Find the two snapshots bracketing targetTime
        const WorldSnapshot* before = nullptr;
        const WorldSnapshot* after = nullptr;

        for (size_t i = 0; i + 1 < snapshotBuffer_.size(); ++i) {
            if (snapshotBuffer_[i].timestamp <= targetTime &&
                snapshotBuffer_[i + 1].timestamp >= targetTime) {
                before = &snapshotBuffer_[i];
                after = &snapshotBuffer_[i + 1];
                break;
            }
        }

        std::vector<EntitySnapshot> result;
        result.reserve(entities_.size());

        if (!before || !after || before->timestamp == after->timestamp) {
            // No interpolation possible, return current state
            for (const auto& [id, data] : entities_) {
                if (data.registered) {
                    result.push_back(data.current);
                }
            }
            return result;
        }

        float t = (targetTime - before->timestamp) / (after->timestamp - before->timestamp);
        t = std::clamp(t, 0.0f, 1.0f);

        // Build a map of 'before' entities for lookup
        std::unordered_map<NetEntityId, const EntitySnapshot*> beforeMap;
        for (const auto& es : before->entities) {
            beforeMap[es.entityId] = &es;
        }

        for (const auto& afterEntity : after->entities) {
            auto bIt = beforeMap.find(afterEntity.entityId);
            if (bIt == beforeMap.end()) {
                result.push_back(afterEntity);
                continue;
            }

            const auto& b = *bIt->second;
            EntitySnapshot interp;
            interp.entityId = afterEntity.entityId;
            interp.position = glm::mix(b.position, afterEntity.position, t);
            interp.rotation = glm::mix(b.rotation, afterEntity.rotation, t);
            interp.scale    = glm::mix(b.scale, afterEntity.scale, t);
            interp.timestamp = targetTime;
            result.push_back(interp);
        }

        return result;
    }

    const EntitySnapshot* EntityReplication::getEntityState(NetEntityId id) const {
        auto it = entities_.find(id);
        if (it == entities_.end() || !it->second.registered) { return nullptr; }
        return &it->second.current;
    }

} // namespace Nox

// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <cstdint>
#include <vector>

namespace Nox {

template<typename T>
class HandlePool {
public:
    struct Handle {
        uint32_t index      = 0;
        uint32_t generation = 0;

        [[nodiscard]] bool valid() const { return generation != 0; }

        bool operator==(const Handle&) const = default;
    };

    Handle insert(T value) {
        uint32_t idx;
        if (!freeList_.empty()) {
            idx = freeList_.back();
            freeList_.pop_back();
            slots_[idx].value = std::move(value);
            slots_[idx].alive = true;
        }
        else {
            idx = static_cast<uint32_t>(slots_.size());
            slots_.push_back(Slot{
                .value      = std::move(value),
                .generation = 1,
                .alive      = true
            });
        }
        return Handle{ .index = idx, .generation = slots_[idx].generation };
    }

    [[nodiscard]] T* get(Handle handle) {
        if (handle.index >= slots_.size()) return nullptr;
        auto& slot = slots_[handle.index];
        if (!slot.alive || slot.generation != handle.generation) return nullptr;
        return &slot.value;
    }

    [[nodiscard]] const T* get(Handle handle) const {
        if (handle.index >= slots_.size()) return nullptr;
        const auto& slot = slots_[handle.index];
        if (!slot.alive || slot.generation != handle.generation) return nullptr;
        return &slot.value;
    }

    void remove(Handle handle) {
        if (handle.index >= slots_.size()) return;
        auto& slot = slots_[handle.index];
        if (!slot.alive || slot.generation != handle.generation) return;
        slot.alive = false;
        ++slot.generation;
        freeList_.push_back(handle.index);
    }

    [[nodiscard]] size_t size() const {
        return slots_.size() - freeList_.size();
    }

private:
    struct Slot {
        T        value{};
        uint32_t generation = 1;
        bool     alive      = false;
    };

    std::vector<Slot>     slots_;
    std::vector<uint32_t> freeList_;
};

} // namespace Nox

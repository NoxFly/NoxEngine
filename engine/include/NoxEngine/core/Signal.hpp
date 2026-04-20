// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

namespace Nox {

    template<typename... Args>
    class Signal {
    public:
        uint32_t connect(std::function<void(Args...)> slot) {
            uint32_t id = nextId_++;
            slots_.emplace_back(id, std::move(slot));
            return id;
        }

        void disconnect(uint32_t id) {
            std::erase_if(slots_, [id](const auto& pair) { return pair.first == id; });
        }

        void emit(Args... args) const {
            for (const auto& [id, fn] : slots_) {
                fn(args...);
            }
        }

    private:
        std::vector<std::pair<uint32_t, std::function<void(Args...)>>> slots_;
        uint32_t nextId_ = 1;
    };

} // namespace Nox

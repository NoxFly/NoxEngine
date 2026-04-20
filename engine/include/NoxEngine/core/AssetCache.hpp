// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <filesystem>
#include <mutex>
#include <string>
#include <unordered_map>

namespace Nox {

    template<typename T>
    class AssetCache {
    public:
        AssetCache() = default;

        [[nodiscard]] bool contains(const std::filesystem::path& path) const {
            auto canonical = std::filesystem::weakly_canonical(path).string();
            return cache_.contains(canonical);
        }

        [[nodiscard]] T get(const std::filesystem::path& path) const {
            auto canonical = std::filesystem::weakly_canonical(path).string();
            auto it = cache_.find(canonical);
            if (it != cache_.end()) {
                return it->second;
            }
            return T{};
        }

        void store(const std::filesystem::path& path, T value) {
            auto canonical = std::filesystem::weakly_canonical(path).string();
            cache_[canonical] = std::move(value);
        }

        void remove(const std::filesystem::path& path) {
            auto canonical = std::filesystem::weakly_canonical(path).string();
            cache_.erase(canonical);
        }

        void clear() {
            cache_.clear();
        }

    private:
        std::unordered_map<std::string, T> cache_;
    };

} // namespace Nox

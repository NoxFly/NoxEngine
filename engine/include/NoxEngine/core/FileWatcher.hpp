// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/core/Signal.hpp>

#include <chrono>
#include <filesystem>
#include <string>
#include <unordered_map>

namespace Nox {

    class FileWatcher {
    public:
        FileWatcher() = default;

        void watch(const std::filesystem::path& path);
        void unwatch(const std::filesystem::path& path);

        // Call periodically (e.g., each frame). Emits onFileChanged for modified files.
        void poll();

        Signal<const std::filesystem::path&> onFileChanged;

    private:
        using Clock = std::filesystem::file_time_type;
        std::unordered_map<std::string, Clock> watchedFiles_;
    };

} // namespace Nox

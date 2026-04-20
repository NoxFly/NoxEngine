// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/core/FileWatcher.hpp>
#include <NoxEngine/core/Logger.hpp>

namespace Nox {

    void FileWatcher::watch(const std::filesystem::path& path) {
        if (!std::filesystem::exists(path)) {
            NOX_LOG_WARN("FileWatcher: path does not exist: {}", path.string());
            return;
        }
        auto canonical = std::filesystem::weakly_canonical(path).string();
        watchedFiles_[canonical] = std::filesystem::last_write_time(path);
    }

    void FileWatcher::unwatch(const std::filesystem::path& path) {
        auto canonical = std::filesystem::weakly_canonical(path).string();
        watchedFiles_.erase(canonical);
    }

    void FileWatcher::poll() {
        for (auto& [pathStr, lastTime] : watchedFiles_) {
            std::filesystem::path p(pathStr);
            if (!std::filesystem::exists(p)) {
                continue;
            }
            auto currentTime = std::filesystem::last_write_time(p);
            if (currentTime != lastTime) {
                lastTime = currentTime;
                onFileChanged.emit(p);
            }
        }
    }

} // namespace Nox

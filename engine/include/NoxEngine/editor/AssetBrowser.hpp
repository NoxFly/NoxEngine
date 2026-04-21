// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/core/Signal.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace Nox {

    /// ImGui panel that browses the filesystem for assets.
    /// Displays folders and files with extension-based icons.
    class AssetBrowser {
    public:
        explicit AssetBrowser(const std::filesystem::path& rootDir = ".");

        /// Draw the asset browser panel. Call inside an ImGui frame.
        void draw();

        /// Set the root directory for browsing.
        void setRootDirectory(const std::filesystem::path& dir);

        /// The currently selected file path (empty if none).
        [[nodiscard]] const std::filesystem::path& selectedPath() const { return selected_; }

        /// Fired when the user double-clicks an asset file.
        Signal<std::filesystem::path> onAssetSelected;

    private:
        void navigateTo(const std::filesystem::path& dir);

        struct Entry {
            std::filesystem::path path;
            std::string           name;
            bool                  isDirectory = false;
        };

        void refreshEntries();

        std::filesystem::path root_;
        std::filesystem::path current_;
        std::filesystem::path selected_;
        std::vector<Entry>    entries_;
        bool                  needsRefresh_ = true;
    };

} // namespace Nox

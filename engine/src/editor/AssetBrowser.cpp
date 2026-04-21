// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/editor/AssetBrowser.hpp>
#include <NoxEngine/core/Logger.hpp>

#ifdef NOX_HAS_IMGUI
#include <imgui.h>
#endif

#include <algorithm>

namespace Nox {

    AssetBrowser::AssetBrowser(const std::filesystem::path& rootDir)
        : root_(std::filesystem::absolute(rootDir))
        , current_(root_) {
    }

    void AssetBrowser::setRootDirectory(const std::filesystem::path& dir) {
        root_ = std::filesystem::absolute(dir);
        current_ = root_;
        needsRefresh_ = true;
    }

    void AssetBrowser::draw() {
#ifdef NOX_HAS_IMGUI
        ImGui::SetNextWindowPos(ImVec2(600, 250), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_FirstUseEver);

        if (ImGui::Begin("Asset Browser")) {
            // Navigation bar
            if (current_ != root_) {
                if (ImGui::Button("<- Back")) {
                    navigateTo(current_.parent_path());
                }
                ImGui::SameLine();
            }

            // Show relative path from root
            auto relPath = std::filesystem::relative(current_, root_);
            ImGui::Text("/ %s", relPath.string().c_str());
            ImGui::Separator();

            // Refresh if needed
            if (needsRefresh_) {
                refreshEntries();
                needsRefresh_ = false;
            }

            // Display entries
            for (const auto& entry : entries_) {
                bool isSelected = (selected_ == entry.path);

                if (entry.isDirectory) {
                    std::string label = "[DIR] " + entry.name;
                    if (ImGui::Selectable(label.c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick)) {
                        selected_ = entry.path;
                        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                            navigateTo(entry.path);
                        }
                    }
                }
                else {
                    // File icon based on extension
                    auto ext = entry.path.extension().string();
                    const char* icon = "    ";
                    if (ext == ".glb" || ext == ".gltf") { icon = "[3D]"; }
                    else if (ext == ".png" || ext == ".jpg" || ext == ".bmp" || ext == ".tga") { icon = "[TX]"; }
                    else if (ext == ".vert" || ext == ".frag" || ext == ".glsl") { icon = "[SH]"; }
                    else if (ext == ".json") { icon = "[JS]"; }
                    else if (ext == ".nox") { icon = "[SC]"; }

                    std::string label = std::string(icon) + " " + entry.name;
                    if (ImGui::Selectable(label.c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick)) {
                        selected_ = entry.path;
                        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                            onAssetSelected.emit(entry.path);
                        }
                    }
                }
            }

            // Show selected file info
            if (!selected_.empty() && !std::filesystem::is_directory(selected_)) {
                ImGui::Separator();
                ImGui::Text("Selected: %s", selected_.filename().string().c_str());
                auto ext = selected_.extension().string();
                ImGui::Text("Type: %s", ext.c_str());
                if (std::filesystem::exists(selected_)) {
                    auto size = std::filesystem::file_size(selected_);
                    if (size < 1024) {
                        ImGui::Text("Size: %llu B", static_cast<unsigned long long>(size));
                    }
                    else if (size < 1024 * 1024) {
                        ImGui::Text("Size: %.1f KB", static_cast<float>(size) / 1024.0f);
                    }
                    else {
                        ImGui::Text("Size: %.1f MB", static_cast<float>(size) / (1024.0f * 1024.0f));
                    }
                }
            }
        }
        ImGui::End();
#endif
    }

    void AssetBrowser::navigateTo(const std::filesystem::path& dir) {
        if (std::filesystem::is_directory(dir)) {
            current_ = dir;
            selected_ = std::filesystem::path{};
            needsRefresh_ = true;
        }
    }

    void AssetBrowser::refreshEntries() {
        entries_.clear();

        if (!std::filesystem::exists(current_) || !std::filesystem::is_directory(current_)) {
            return;
        }

        for (const auto& dirEntry : std::filesystem::directory_iterator(current_)) {
            Entry entry;
            entry.path = dirEntry.path();
            entry.name = dirEntry.path().filename().string();
            entry.isDirectory = dirEntry.is_directory();

            // Skip hidden files/folders
            if (!entry.name.empty() && entry.name[0] == '.') {
                continue;
            }

            entries_.push_back(std::move(entry));
        }

        // Sort: directories first, then files, alphabetically
        std::sort(entries_.begin(), entries_.end(),
            [](const Entry& a, const Entry& b) {
                if (a.isDirectory != b.isDirectory) {
                    return a.isDirectory;
                }
                return a.name < b.name;
            });
    }

} // namespace Nox

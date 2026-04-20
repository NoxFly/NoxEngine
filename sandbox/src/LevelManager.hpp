// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include "Level.hpp"
#include <NoxEngine/core/Logger.hpp>

#ifdef NOX_HAS_IMGUI
#include <imgui.h>
#endif

#include <memory>
#include <string>
#include <vector>

class LevelManager {
public:
    LevelManager() = default;

    void addLevel(std::unique_ptr<Level> level) {
        levels_.push_back(std::move(level));
    }

    void init(Nox::Engine& engine) {
        if (!levels_.empty()) {
            levels_[currentIndex_]->setup(engine);
        }
    }

    void switchTo(int index, Nox::Engine& engine) {
        if (index < 0 || index >= static_cast<int>(levels_.size()) || index == currentIndex_) {
            return;
        }
        levels_[currentIndex_]->teardown(engine);
        currentIndex_ = index;
        levels_[currentIndex_]->setup(engine);
    }

    void update(Nox::Engine& engine, float dt) {
        if (levels_.empty()) { return; }
        levels_[currentIndex_]->update(engine, dt);
    }

    void render(Nox::Engine& engine) {
        if (levels_.empty()) { return; }
        auto& level = *levels_[currentIndex_];
        engine.render(level.scene(), level.camera());
    }

    void drawUI([[maybe_unused]] Nox::Engine& engine) {
#ifdef NOX_HAS_IMGUI
        ImGui::SetNextWindowPos(ImVec2(10, 100), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(250, 0), ImGuiCond_FirstUseEver);

        if (ImGui::Begin("Levels")) {
            // Level selector combo
            const char* currentName = levels_[currentIndex_]->name().c_str();
            if (ImGui::BeginCombo("Level", currentName)) {
                for (int i = 0; i < static_cast<int>(levels_.size()); ++i) {
                    bool selected = (i == currentIndex_);
                    if (ImGui::Selectable(levels_[i]->name().c_str(), selected)) {
                        switchTo(i, engine);
                    }
                    if (selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::Separator();

            // Level-specific UI
            levels_[currentIndex_]->drawUI();
        }
        ImGui::End();
#endif
    }

    [[nodiscard]] int currentIndex() const { return currentIndex_; }
    [[nodiscard]] Level* current() { return levels_.empty() ? nullptr : levels_[currentIndex_].get(); }

private:
    std::vector<std::unique_ptr<Level>> levels_;
    int currentIndex_ = 0;
};

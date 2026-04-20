// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/core/DebugOverlay.hpp>

#ifdef NOX_HAS_IMGUI

#include <NoxEngine/core/Engine.hpp>
#include <NoxEngine/platform/Window.hpp>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include <SDL3/SDL.h>

namespace Nox {

    DebugOverlay::DebugOverlay() = default;

    DebugOverlay::~DebugOverlay() {
        if (initialized_) {
            shutdown();
        }
    }

    void DebugOverlay::init(Engine& engine) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

        auto* sdlWindow = engine.window().nativeHandle();
        auto* glContext = engine.window().glContext();

        ImGui_ImplSDL3_InitForOpenGL(sdlWindow, glContext);
        ImGui_ImplOpenGL3_Init("#version 460");

        initialized_ = true;
    }

    void DebugOverlay::beginFrame() {
        if (!initialized_) return;
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    void DebugOverlay::render(const Engine& engine) {
        if (!initialized_ || !visible_) return;

        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(200, 80), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.5f);

        if (ImGui::Begin("Debug", &visible_, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
            ImGui::Text("FPS: %.1f", static_cast<double>(engine.fps()));
            ImGui::Text("Frame: %.2f ms", static_cast<double>(engine.frameTime() * 1000.0f));
        }
        ImGui::End();
    }

    void DebugOverlay::endFrame() {
        if (!initialized_) return;
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void DebugOverlay::shutdown() {
        if (!initialized_) return;
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        initialized_ = false;
    }

} // namespace Nox

#else // NOX_HAS_IMGUI not defined — stub implementations

namespace Nox {

    DebugOverlay::DebugOverlay() = default;
    DebugOverlay::~DebugOverlay() = default;
    void DebugOverlay::init([[maybe_unused]] Engine& engine) {}
    void DebugOverlay::beginFrame() {}
    void DebugOverlay::render([[maybe_unused]] const Engine& engine) {}
    void DebugOverlay::endFrame() {}
    void DebugOverlay::shutdown() {}

} // namespace Nox

#endif

// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

namespace Nox {

    class Engine;

    class DebugOverlay {
    public:
        DebugOverlay();
        ~DebugOverlay();

        DebugOverlay(const DebugOverlay&) = delete;
        DebugOverlay& operator=(const DebugOverlay&) = delete;
        DebugOverlay(DebugOverlay&&) = delete;
        DebugOverlay& operator=(DebugOverlay&&) = delete;

        void init(Engine& engine);
        void beginFrame();
        void render(const Engine& engine);
        void endFrame();
        void shutdown();

        void setVisible(bool visible) { visible_ = visible; }
        [[nodiscard]] bool isVisible() const { return visible_; }

    private:
        bool initialized_ = false;
        bool visible_      = true;
    };

} // namespace Nox

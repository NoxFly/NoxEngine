// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/core/Signal.hpp>
#include <NoxEngine/platform/Input.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

struct SDL_Window;

namespace Nox {

    class Window {
    public:
        Window(std::string_view title, int width, int height);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        [[nodiscard]] bool pollEvents();
        void swapBuffers();

        [[nodiscard]] std::pair<int, int> size() const { return { width_, height_ }; }
        [[nodiscard]] int width()  const { return width_; }
        [[nodiscard]] int height() const { return height_; }
        [[nodiscard]] float aspect() const;

        [[nodiscard]] SDL_Window* nativeHandle() const { return window_; }
        [[nodiscard]] void* glContext() const { return glContext_; }

        [[nodiscard]] Input& input() { return input_; }
        [[nodiscard]] const Input& input() const { return input_; }

        Signal<int, int> onResize;
        Signal<>         onClose;

    private:
        SDL_Window* window_  = nullptr;
        void*       glContext_ = nullptr;
        int         width_   = 0;
        int         height_  = 0;
        Input       input_;
    };

} // namespace Nox

// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Nox {

    class Scene3D;

    /// Lua scripting engine powered by sol2.
    /// Registers engine types and exposes them to Lua scripts.
    class LuaScriptEngine {
    public:
        LuaScriptEngine();
        ~LuaScriptEngine();

        LuaScriptEngine(const LuaScriptEngine&) = delete;
        LuaScriptEngine& operator=(const LuaScriptEngine&) = delete;
        LuaScriptEngine(LuaScriptEngine&&) = default;
        LuaScriptEngine& operator=(LuaScriptEngine&&) = default;

        /// Initialize the Lua state and register engine types.
        [[nodiscard]] bool init();

        /// Shutdown and destroy the Lua state.
        void shutdown();

        /// Load and execute a Lua script file.
        [[nodiscard]] bool loadScript(const std::filesystem::path& path);

        /// Execute a Lua string.
        [[nodiscard]] bool execute(std::string_view luaCode);

        /// Call a global Lua function with float dt (per-frame update).
        void callUpdate(float dt);

        /// Call a named global Lua function (no arguments).
        void callFunction(std::string_view name);

        /// Set a global variable in Lua.
        void setGlobal(std::string_view name, float value);
        void setGlobal(std::string_view name, int value);
        void setGlobal(std::string_view name, bool value);
        void setGlobal(std::string_view name, std::string_view value);

        /// Register a C++ function callable from Lua.
        void registerFunction(std::string_view name, std::function<void()> fn);

        /// Hot-reload a script (re-execute).
        [[nodiscard]] bool reloadScript(const std::filesystem::path& path);

        [[nodiscard]] bool isInitialized() const { return initialized_; }

        /// List all loaded script paths.
        [[nodiscard]] const std::vector<std::filesystem::path>& loadedScripts() const;

    private:
        void registerEngineTypes();

        struct Impl;
        std::unique_ptr<Impl> impl_;
        bool initialized_ = false;
        std::vector<std::filesystem::path> loadedScripts_;
    };

} // namespace Nox

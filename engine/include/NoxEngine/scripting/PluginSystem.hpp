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

    /// Plugin interface. Dynamically loaded modules implement this.
    class IPlugin {
    public:
        virtual ~IPlugin() = default;
        virtual std::string_view name() const = 0;
        virtual std::string_view version() const = 0;
        virtual bool onLoad() = 0;
        virtual void onUnload() = 0;
        virtual void onUpdate(float dt) = 0;
    };

    /// Factory function type exported by plugin shared libraries.
    /// The DLL must export: extern "C" Nox::IPlugin* noxCreatePlugin();
    using PluginFactoryFn = IPlugin* (*)();

    /// Manages dynamically loaded plugin modules (.dll / .so).
    class PluginSystem {
    public:
        PluginSystem() = default;
        ~PluginSystem();

        PluginSystem(const PluginSystem&) = delete;
        PluginSystem& operator=(const PluginSystem&) = delete;
        PluginSystem(PluginSystem&&) = default;
        PluginSystem& operator=(PluginSystem&&) = default;

        /// Load a plugin from a shared library path.
        [[nodiscard]] bool loadPlugin(const std::filesystem::path& path);

        /// Unload a plugin by name.
        void unloadPlugin(std::string_view name);

        /// Unload all plugins.
        void unloadAll();

        /// Reload a plugin (unload + load). Useful for hot-reload.
        [[nodiscard]] bool reloadPlugin(std::string_view name);

        /// Update all loaded plugins.
        void update(float dt);

        /// Get a loaded plugin by name.
        [[nodiscard]] IPlugin* getPlugin(std::string_view name);

        /// List all loaded plugin names.
        [[nodiscard]] std::vector<std::string> pluginNames() const;

    private:
        struct LoadedPlugin {
            std::filesystem::path path;
            void* libraryHandle = nullptr;
            std::unique_ptr<IPlugin> instance;
        };

        std::unordered_map<std::string, LoadedPlugin> plugins_;
    };

} // namespace Nox

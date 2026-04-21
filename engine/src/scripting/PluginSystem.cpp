// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/scripting/PluginSystem.hpp>
#include <NoxEngine/core/Logger.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace Nox {

    namespace {
        void* loadLibrary(const std::filesystem::path& path) {
#ifdef _WIN32
            return LoadLibraryW(path.wstring().c_str());
#else
            return dlopen(path.string().c_str(), RTLD_NOW);
#endif
        }

        void freeLibrary(void* handle) {
            if (!handle) { return; }
#ifdef _WIN32
            FreeLibrary(static_cast<HMODULE>(handle));
#else
            dlclose(handle);
#endif
        }

        void* getSymbol(void* handle, const char* name) {
#ifdef _WIN32
            return reinterpret_cast<void*>(
                GetProcAddress(static_cast<HMODULE>(handle), name));
#else
            return dlsym(handle, name);
#endif
        }
    }

    PluginSystem::~PluginSystem() {
        unloadAll();
    }

    bool PluginSystem::loadPlugin(const std::filesystem::path& path) {
        void* lib = loadLibrary(path);
        if (!lib) {
            NOX_LOG_ERROR("Failed to load plugin library: {}", path.string());
            return false;
        }

        auto factoryFn = reinterpret_cast<PluginFactoryFn>(getSymbol(lib, "noxCreatePlugin"));
        if (!factoryFn) {
            NOX_LOG_ERROR("Plugin '{}' has no noxCreatePlugin export", path.string());
            freeLibrary(lib);
            return false;
        }

        IPlugin* raw = factoryFn();
        if (!raw) {
            NOX_LOG_ERROR("Plugin factory returned null: {}", path.string());
            freeLibrary(lib);
            return false;
        }

        std::unique_ptr<IPlugin> plugin(raw);
        std::string pluginName(plugin->name());

        if (plugins_.contains(pluginName)) {
            NOX_LOG_WARN("Plugin '{}' already loaded — skipping", pluginName);
            freeLibrary(lib);
            return false;
        }

        if (!plugin->onLoad()) {
            NOX_LOG_ERROR("Plugin '{}' onLoad failed", pluginName);
            freeLibrary(lib);
            return false;
        }

        NOX_LOG_INFO("Loaded plugin '{}' v{}", pluginName, plugin->version());
        plugins_[pluginName] = { path, lib, std::move(plugin) };
        return true;
    }

    void PluginSystem::unloadPlugin(std::string_view name) {
        auto key = std::string(name);
        auto it = plugins_.find(key);
        if (it == plugins_.end()) { return; }

        it->second.instance->onUnload();
        it->second.instance.reset();
        freeLibrary(it->second.libraryHandle);
        plugins_.erase(it);
        NOX_LOG_INFO("Unloaded plugin '{}'", name);
    }

    void PluginSystem::unloadAll() {
        for (auto& [name, plugin] : plugins_) {
            plugin.instance->onUnload();
            plugin.instance.reset();
            freeLibrary(plugin.libraryHandle);
        }
        plugins_.clear();
    }

    bool PluginSystem::reloadPlugin(std::string_view name) {
        auto key = std::string(name);
        auto it = plugins_.find(key);
        if (it == plugins_.end()) { return false; }

        auto path = it->second.path;
        unloadPlugin(name);
        return loadPlugin(path);
    }

    void PluginSystem::update(float dt) {
        for (auto& [name, plugin] : plugins_) {
            plugin.instance->onUpdate(dt);
        }
    }

    IPlugin* PluginSystem::getPlugin(std::string_view name) {
        auto key = std::string(name);
        auto it = plugins_.find(key);
        return (it != plugins_.end()) ? it->second.instance.get() : nullptr;
    }

    std::vector<std::string> PluginSystem::pluginNames() const {
        std::vector<std::string> names;
        names.reserve(plugins_.size());
        for (const auto& [name, _] : plugins_) {
            names.push_back(name);
        }
        return names;
    }

} // namespace Nox

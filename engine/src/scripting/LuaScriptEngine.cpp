// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/scripting/LuaScriptEngine.hpp>
#include <NoxEngine/core/Logger.hpp>
#include <NoxEngine/math/Types.hpp>

#if defined(NOX_HAS_LUA) && defined(NOX_HAS_SOL2)
#define NOX_LUA_AVAILABLE
#include <sol/sol.hpp>
#endif

namespace Nox {

    struct LuaScriptEngine::Impl {
#ifdef NOX_LUA_AVAILABLE
        sol::state lua;
#endif
    };

    LuaScriptEngine::LuaScriptEngine() = default;

    LuaScriptEngine::~LuaScriptEngine() {
        shutdown();
    }

    bool LuaScriptEngine::init() {
#ifdef NOX_LUA_AVAILABLE
        if (initialized_) { return true; }

        impl_ = std::make_unique<Impl>();
        impl_->lua.open_libraries(
            sol::lib::base,
            sol::lib::math,
            sol::lib::string,
            sol::lib::table,
            sol::lib::io
        );

        registerEngineTypes();

        initialized_ = true;
        NOX_LOG_INFO("Lua scripting engine initialized");
        return true;
#else
        NOX_LOG_WARN("Lua scripting not available (Lua/sol2 not linked)");
        return false;
#endif
    }

    void LuaScriptEngine::shutdown() {
#ifdef NOX_LUA_AVAILABLE
        if (!initialized_) { return; }
        impl_.reset();
        loadedScripts_.clear();
        initialized_ = false;
        NOX_LOG_INFO("Lua scripting engine shut down");
#endif
    }

    bool LuaScriptEngine::loadScript([[maybe_unused]] const std::filesystem::path& path) {
#ifdef NOX_LUA_AVAILABLE
        if (!initialized_) { return false; }

        auto result = impl_->lua.safe_script_file(path.string(), sol::script_pass_on_error);
        if (!result.valid()) {
            sol::error err = result;
            NOX_LOG_ERROR("Lua script error in '{}': {}", path.string(), err.what());
            return false;
        }

        loadedScripts_.push_back(path);
        NOX_LOG_INFO("Loaded Lua script: {}", path.filename().string());
        return true;
#else
        return false;
#endif
    }

    bool LuaScriptEngine::execute([[maybe_unused]] std::string_view luaCode) {
#ifdef NOX_LUA_AVAILABLE
        if (!initialized_) { return false; }

        auto result = impl_->lua.safe_script(luaCode, sol::script_pass_on_error);
        if (!result.valid()) {
            sol::error err = result;
            NOX_LOG_ERROR("Lua execution error: {}", err.what());
            return false;
        }
        return true;
#else
        return false;
#endif
    }

    void LuaScriptEngine::callUpdate([[maybe_unused]] float dt) {
#ifdef NOX_LUA_AVAILABLE
        if (!initialized_) { return; }

        sol::protected_function fn = impl_->lua["onUpdate"];
        if (fn.valid()) {
            auto result = fn(dt);
            if (!result.valid()) {
                sol::error err = result;
                NOX_LOG_ERROR("Lua onUpdate error: {}", err.what());
            }
        }
#endif
    }

    void LuaScriptEngine::callFunction([[maybe_unused]] std::string_view name) {
#ifdef NOX_LUA_AVAILABLE
        if (!initialized_) { return; }

        sol::protected_function fn = impl_->lua[std::string(name)];
        if (fn.valid()) {
            auto result = fn();
            if (!result.valid()) {
                sol::error err = result;
                NOX_LOG_ERROR("Lua function '{}' error: {}", name, err.what());
            }
        }
#endif
    }

    void LuaScriptEngine::setGlobal([[maybe_unused]] std::string_view name,
                                     [[maybe_unused]] float value) {
#ifdef NOX_LUA_AVAILABLE
        if (initialized_) { impl_->lua[std::string(name)] = value; }
#endif
    }

    void LuaScriptEngine::setGlobal([[maybe_unused]] std::string_view name,
                                     [[maybe_unused]] int value) {
#ifdef NOX_LUA_AVAILABLE
        if (initialized_) { impl_->lua[std::string(name)] = value; }
#endif
    }

    void LuaScriptEngine::setGlobal([[maybe_unused]] std::string_view name,
                                     [[maybe_unused]] bool value) {
#ifdef NOX_LUA_AVAILABLE
        if (initialized_) { impl_->lua[std::string(name)] = value; }
#endif
    }

    void LuaScriptEngine::setGlobal([[maybe_unused]] std::string_view name,
                                     [[maybe_unused]] std::string_view value) {
#ifdef NOX_LUA_AVAILABLE
        if (initialized_) { impl_->lua[std::string(name)] = std::string(value); }
#endif
    }

    void LuaScriptEngine::registerFunction([[maybe_unused]] std::string_view name,
                                            [[maybe_unused]] std::function<void()> fn) {
#ifdef NOX_LUA_AVAILABLE
        if (initialized_) { impl_->lua.set_function(std::string(name), fn); }
#endif
    }

    bool LuaScriptEngine::reloadScript(const std::filesystem::path& path) {
        return loadScript(path);
    }

    const std::vector<std::filesystem::path>& LuaScriptEngine::loadedScripts() const {
        return loadedScripts_;
    }

    void LuaScriptEngine::registerEngineTypes() {
#ifdef NOX_LUA_AVAILABLE
        // Register Vec3 type
        impl_->lua.new_usertype<Math::Vec3>("Vec3",
            sol::constructors<Math::Vec3(), Math::Vec3(float, float, float)>(),
            "x", &Math::Vec3::x,
            "y", &Math::Vec3::y,
            "z", &Math::Vec3::z
        );

        // Engine log function
        impl_->lua.set_function("log_info", [](const std::string& msg) {
            NOX_LOG_INFO("[Lua] {}", msg);
        });

        impl_->lua.set_function("log_warn", [](const std::string& msg) {
            NOX_LOG_WARN("[Lua] {}", msg);
        });

        impl_->lua.set_function("log_error", [](const std::string& msg) {
            NOX_LOG_ERROR("[Lua] {}", msg);
        });
#endif
    }

} // namespace Nox

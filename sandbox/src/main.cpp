// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/NoxEngine.hpp>

#include "LevelManager.hpp"
#include "levels/LevelGeometry.hpp"
#include "levels/LevelPostProcess.hpp"
#include "levels/LevelAnimation.hpp"
#include "levels/LevelPhysicsBasic.hpp"
#include "levels/LevelPhysicsMarbles.hpp"
#include "levels/LevelEditor.hpp"
#include "levels/LevelSky.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    using namespace Nox;

    EngineConfig config;
    config.title  = "NoxEngine Sandbox";
    config.width  = 1280;
    config.height = 720;
    config.vsync  = true;

    Engine engine(config);

    // Register all test levels
    LevelManager levels;
    levels.addLevel(std::make_unique<LevelGeometry>());
    levels.addLevel(std::make_unique<LevelPostProcess>());
    levels.addLevel(std::make_unique<LevelAnimation>());
    levels.addLevel(std::make_unique<LevelPhysicsBasic>());
    levels.addLevel(std::make_unique<LevelPhysicsMarbles>());
    levels.addLevel(std::make_unique<LevelEditor>());
    levels.addLevel(std::make_unique<LevelSky>());
    levels.init(engine);

    engine.run([&](float dt) {
        auto& input = engine.input();

        if (input.isKeyPressed(Key::Escape)) {
            engine.stop();
            return;
        }

        if (input.isKeyPressed(Key::F3)) {
            engine.debugOverlay().setVisible(!engine.debugOverlay().isVisible());
        }

        levels.update(engine, dt);
        levels.drawUI(engine);
        levels.render(engine);
    });

    return 0;
}

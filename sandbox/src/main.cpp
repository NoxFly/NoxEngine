// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/NoxEngine.hpp>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    using namespace Nox;

    // ── Engine + window ────────────────────────────────────────
    EngineConfig config;
    config.title  = "NoxEngine Sandbox";
    config.width  = 1280;
    config.height = 720;
    config.vsync  = true;

    Engine engine(config);

    // ── Scene ──────────────────────────────────────────────────
    Scene3D scene;

    // ── Camera ─────────────────────────────────────────────────
    PerspectiveCamera camera(45.0f, engine.aspect(), 0.1f, 1000.0f);
    camera.setPosition(3.0f, 2.0f, 3.0f);
    camera.lookAt(0.0f, 0.0f, 0.0f);

    // ── Geometry + Material + Mesh ─────────────────────────────
    auto box = Geometry::box(1.0f, 1.0f, 1.0f);
    auto mat = Material::standard();
    mat->setColor(Color(0.8f, 0.4f, 0.2f));

    auto mesh = std::make_shared<Mesh>(box, mat);
    mesh->setPosition(0.0f, 0.5f, 0.0f);
    scene.add(mesh);

    // Floor
    auto floorGeom = Geometry::plane(10.0f, 10.0f);
    auto floorMat  = Material::standard();
    floorMat->setColor(Color(0.3f, 0.3f, 0.35f));

    auto floor = std::make_shared<Mesh>(floorGeom, floorMat);
    scene.add(floor);

    // ── Lights ─────────────────────────────────────────────────
    auto sun = std::make_shared<DirectionalLight>(Color::White, 1.0f);
    sun->setDirection(-1.0f, -1.0f, -0.5f);
    scene.add(sun);

    auto ambient = std::make_shared<AmbientLight>(Color::White, 0.08f);
    scene.add(ambient);

    // ── Main loop ──────────────────────────────────────────────
    engine.run([&](float dt) {
        mesh->rotate(0.0f, 90.0f * dt, 0.0f);
        engine.render(scene, camera);
    });

    return 0;
}

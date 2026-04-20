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
    camera.setPosition(5.0f, 3.0f, 5.0f);
    camera.lookAt(0.0f, 0.0f, 0.0f);

    // Camera controllers — switchable with keys 1/2/3
    OrbitCameraController       orbitCtrl(camera);
    FlyCameraController         flyCtrl(camera);
    FirstPersonCameraController fpCtrl(camera);
    int activeCameraMode = 1; // 1=Orbit, 2=Fly, 3=FirstPerson

    // ── Geometries showcase ────────────────────────────────────
    // v0.1: box
    auto boxMat = Material::standard();
    boxMat->setColor(Color(0.8f, 0.4f, 0.2f));
    boxMat->setRoughness(0.3f);
    auto boxMesh = std::make_shared<Mesh>(Geometry::box(1.0f, 1.0f, 1.0f), boxMat);
    boxMesh->setName("Box");
    boxMesh->setPosition(-3.0f, 0.5f, 0.0f);
    scene.add(boxMesh);

    // v0.1: sphere
    auto sphereMat = Material::standard();
    sphereMat->setColor(Color(0.2f, 0.6f, 0.9f));
    sphereMat->setRoughness(0.1f);
    sphereMat->setMetallic(0.8f);
    auto sphereMesh = std::make_shared<Mesh>(Geometry::sphere(0.6f, 32, 16), sphereMat);
    sphereMesh->setName("Sphere");
    sphereMesh->setPosition(0.0f, 0.6f, 0.0f);
    scene.add(sphereMesh);

    // v0.2: cylinder
    auto cylMat = Material::standard();
    cylMat->setColor(Color(0.3f, 0.8f, 0.3f));
    cylMat->setRoughness(0.5f);
    auto cylMesh = std::make_shared<Mesh>(Geometry::cylinder(0.4f, 0.4f, 1.2f, 24), cylMat);
    cylMesh->setName("Cylinder");
    cylMesh->setPosition(3.0f, 0.6f, 0.0f);
    scene.add(cylMesh);

    // v0.2: unlit material
    auto unlitMat = Material::unlit();
    unlitMat->setColor(Color(1.0f, 1.0f, 0.0f));
    auto unlitMesh = std::make_shared<Mesh>(Geometry::box(0.5f, 0.5f, 0.5f), unlitMat);
    unlitMesh->setName("UnlitCube");
    unlitMesh->setPosition(0.0f, 2.0f, -3.0f);
    scene.add(unlitMesh);

    // v0.2: custom geometry (fromVertices) — a simple triangle
    std::vector<Vertex> triVerts = {
        { .position = { -0.5f, 0.0f, 0.0f }, .normal = { 0.0f, 0.0f, 1.0f }, .uv = { 0.0f, 0.0f } },
        { .position = {  0.5f, 0.0f, 0.0f }, .normal = { 0.0f, 0.0f, 1.0f }, .uv = { 1.0f, 0.0f } },
        { .position = {  0.0f, 1.0f, 0.0f }, .normal = { 0.0f, 0.0f, 1.0f }, .uv = { 0.5f, 1.0f } },
    };
    std::vector<uint32_t> triIndices = { 0, 1, 2 };
    auto triMat = Material::standard();
    triMat->setColor(Color(0.9f, 0.2f, 0.7f));
    auto triMesh = std::make_shared<Mesh>(Geometry::fromVertices(triVerts, triIndices), triMat);
    triMesh->setName("CustomTriangle");
    triMesh->setPosition(-3.0f, 0.0f, -3.0f);
    scene.add(triMesh);

    // Floor (v0.1: plane)
    auto floorMat = Material::standard();
    floorMat->setColor(Color(0.3f, 0.3f, 0.35f));
    floorMat->setRoughness(0.9f);
    auto floorMesh = std::make_shared<Mesh>(Geometry::plane(20.0f, 20.0f), floorMat);
    floorMesh->setName("Floor");
    scene.add(floorMesh);

    // ── v0.4: PBR material showcase ────────────────────────────
    // Metallic sphere (gold-like)
    auto goldMat = Material::standard();
    goldMat->setColor(Color(1.0f, 0.765f, 0.336f)); // Gold color
    goldMat->setMetallic(1.0f);
    goldMat->setRoughness(0.1f);
    auto goldSphere = std::make_shared<Mesh>(Geometry::sphere(0.5f, 32, 16), goldMat);
    goldSphere->setName("GoldSphere");
    goldSphere->setPosition(-1.5f, 0.5f, 3.0f);
    scene.add(goldSphere);

    // Rough dielectric sphere (clay-like)
    auto clayMat = Material::standard();
    clayMat->setColor(Color(0.8f, 0.5f, 0.3f));
    clayMat->setMetallic(0.0f);
    clayMat->setRoughness(0.95f);
    auto claySphere = std::make_shared<Mesh>(Geometry::sphere(0.5f, 32, 16), clayMat);
    claySphere->setName("ClaySphere");
    claySphere->setPosition(1.5f, 0.5f, 3.0f);
    scene.add(claySphere);

    // Emissive cube (glowing)
    auto emissiveMat = Material::standard();
    emissiveMat->setColor(Color(0.1f, 0.1f, 0.1f));
    emissiveMat->setEmissiveColor(Color(1.0f, 0.3f, 0.1f));
    emissiveMat->setEmissiveIntensity(3.0f);
    emissiveMat->setRoughness(0.5f);
    auto emissiveCube = std::make_shared<Mesh>(Geometry::box(0.4f, 0.4f, 0.4f), emissiveMat);
    emissiveCube->setName("EmissiveCube");
    emissiveCube->setPosition(0.0f, 1.5f, 3.0f);
    scene.add(emissiveCube);

    // PBR roughness gradient row (5 spheres from smooth to rough)
    for (int i = 0; i < 5; ++i) {
        auto mat = Material::standard();
        mat->setColor(Color(0.9f, 0.1f, 0.1f));
        mat->setMetallic(0.5f);
        mat->setRoughness(static_cast<float>(i) / 4.0f * 0.9f + 0.05f);
        auto sphere = std::make_shared<Mesh>(Geometry::sphere(0.3f, 24, 12), mat);
        sphere->setName("RoughnessSphere" + std::to_string(i));
        sphere->setPosition(-4.0f + static_cast<float>(i) * 2.0f, 0.3f, -5.0f);
        scene.add(sphere);
    }

    // ── Lights showcase ────────────────────────────────────────
    // v0.1: directional light
    auto sun = std::make_shared<DirectionalLight>(Color::White, 0.8f);
    sun->setName("Sun");
    sun->setDirection(-1.0f, -1.0f, -0.5f);
    scene.add(sun);

    // v0.1: ambient light
    auto ambient = std::make_shared<AmbientLight>(Color::White, 0.05f);
    ambient->setName("Ambient");
    scene.add(ambient);

    // v0.2: multiple point lights
    auto redLight = std::make_shared<PointLight>(Color(1.0f, 0.2f, 0.1f), 2.0f);
    redLight->setName("RedLight");
    redLight->setPosition(-2.0f, 2.0f, 2.0f);
    redLight->setRange(12.0f);
    scene.add(redLight);

    auto blueLight = std::make_shared<PointLight>(Color(0.1f, 0.3f, 1.0f), 2.0f);
    blueLight->setName("BlueLight");
    blueLight->setPosition(2.0f, 2.0f, -2.0f);
    blueLight->setRange(12.0f);
    scene.add(blueLight);

    auto greenLight = std::make_shared<PointLight>(Color(0.1f, 1.0f, 0.2f), 1.5f);
    greenLight->setName("GreenLight");
    greenLight->setPosition(0.0f, 3.0f, 0.0f);
    greenLight->setRange(15.0f);
    scene.add(greenLight);

    // ── v0.3: Scene graph traversal demo (at startup) ──────────
    // findByName
    auto foundBox = scene.findByName("Box");
    if (foundBox) {
        NOX_LOG_INFO("findByName: found '{}'", foundBox->name());
    }

    // findAllOfType — count meshes & lights
    auto allMeshes = scene.findAllOfType<Mesh>();
    auto allLights = scene.findAllOfType<Light>();
    NOX_LOG_INFO("Scene has {} meshes, {} lights", allMeshes.size(), allLights.size());

    // forEach — log all object names
    scene.forEach<SceneObject>([](SceneObject& obj) {
        NOX_LOG_TRACE("  object: {}", obj.name());
    });

    // ── Main loop ──────────────────────────────────────────────
    float pointLightTimer = 0.0f;

    engine.run([&](float dt) {
        auto& input = engine.input();

        // ── Input: Escape to quit ──────────────────────────────
        if (input.isKeyPressed(Key::Escape)) {
            engine.stop();
            return;
        }

        // ── Input: Switch camera mode with 1/2/3 ──────────────
        if (input.isKeyPressed(Key::Num1)) {
            activeCameraMode = 1;
            NOX_LOG_INFO("Camera: Orbit mode");
        }
        if (input.isKeyPressed(Key::Num2)) {
            activeCameraMode = 2;
            NOX_LOG_INFO("Camera: Fly mode (WASD + right-click look)");
        }
        if (input.isKeyPressed(Key::Num3)) {
            activeCameraMode = 3;
            NOX_LOG_INFO("Camera: First-person mode (WASD + right-click look)");
        }

        // ── Input: Toggle debug overlay with F3 ───────────────
        if (input.isKeyPressed(Key::F3)) {
            engine.debugOverlay().setVisible(!engine.debugOverlay().isVisible());
        }

        // ── Input: Exposure control with PageUp/PageDown ──────
        if (input.isKeyDown(Key::PageUp)) {
            engine.setExposure(engine.exposure() + 1.0f * dt);
            NOX_LOG_INFO("Exposure: {:.2f}", engine.exposure());
        }
        if (input.isKeyDown(Key::PageDown)) {
            engine.setExposure(std::max(0.1f, engine.exposure() - 1.0f * dt));
            NOX_LOG_INFO("Exposure: {:.2f}", engine.exposure());
        }

        // ── Input: Add/remove object with F5/F6 ───────────────
        if (input.isKeyPressed(Key::F5)) {
            auto newSphere = std::make_shared<Mesh>(
                Geometry::sphere(0.3f, 16, 8),
                Material::standard()
            );
            newSphere->setName("DynamicSphere");
            newSphere->setPosition(0.0f, 1.5f, 3.0f);
            newSphere->material()->setColor(Color(1.0f, 0.5f, 0.0f));
            scene.add(newSphere);
            NOX_LOG_INFO("Added DynamicSphere (F6 to remove)");
        }
        if (input.isKeyPressed(Key::F6)) {
            auto found = scene.findByName("DynamicSphere");
            if (found) {
                scene.remove(std::dynamic_pointer_cast<SceneObject>(
                    scene.findByName("DynamicSphere")
                ));
                NOX_LOG_INFO("Removed DynamicSphere");
            }
        }

        // ── Camera controller update ───────────────────────────
        switch (activeCameraMode) {
            case 1: orbitCtrl.update(input, dt);  break;
            case 2: flyCtrl.update(input, dt);    break;
            case 3: fpCtrl.update(input, dt);     break;
            default: break;
        }

        // ── Animate objects ────────────────────────────────────
        boxMesh->rotate(0.0f, 60.0f * dt, 0.0f);
        sphereMesh->rotate(0.0f, 30.0f * dt, 0.0f);
        emissiveCube->rotate(45.0f * dt, 30.0f * dt, 0.0f);

        // Animate point lights in a circle
        pointLightTimer += dt;
        float r = 4.0f;
        redLight->setPosition(
            r * std::cos(pointLightTimer * 0.7f),
            1.5f + std::sin(pointLightTimer * 1.5f),
            r * std::sin(pointLightTimer * 0.7f)
        );
        blueLight->setPosition(
            r * std::cos(pointLightTimer * 0.7f + 2.094f),
            1.5f + std::sin(pointLightTimer * 1.5f + 1.0f),
            r * std::sin(pointLightTimer * 0.7f + 2.094f)
        );

        engine.render(scene, camera);
    });

    return 0;
}

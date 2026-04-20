// Copyright (c) 2026 NoxFly — AGPL-3.0

#include "levels/LevelGeometry.hpp"

#ifdef NOX_HAS_IMGUI
#include <imgui.h>
#endif

using namespace Nox;

void LevelGeometry::setup(Engine& engine) {
    NOX_LOG_INFO("LevelGeometry::setup()");
    camera_.setAspect(engine.aspect());
    camera_.setPosition(5.0f, 3.0f, 5.0f);
    camera_.lookAt(0.0f, 0.0f, 0.0f);
    orbitCtrl_.syncFromCamera();

    // Floor
    auto floorMat = Material::standard();
    floorMat->setColor(Color(0.3f, 0.3f, 0.35f));
    floorMat->setRoughness(0.9f);
    auto floor = std::make_shared<Mesh>(Geometry::plane(20.0f, 20.0f), floorMat);
    floor->setName("Floor");
    scene_.add(floor);

    // Box
    auto boxMat = Material::standard();
    boxMat->setColor(Color(0.8f, 0.4f, 0.2f));
    boxMat->setRoughness(0.3f);
    boxMesh_ = std::make_shared<Mesh>(Geometry::box(1.0f, 1.0f, 1.0f), boxMat);
    boxMesh_->setName("Box");
    boxMesh_->setPosition(-3.0f, 0.5f, 0.0f);
    scene_.add(boxMesh_);

    // Sphere (metallic)
    auto sphereMat = Material::standard();
    sphereMat->setColor(Color(0.2f, 0.6f, 0.9f));
    sphereMat->setRoughness(0.1f);
    sphereMat->setMetallic(0.8f);
    sphereMesh_ = std::make_shared<Mesh>(Geometry::sphere(0.6f, 32, 16), sphereMat);
    sphereMesh_->setName("Sphere");
    sphereMesh_->setPosition(0.0f, 0.6f, 0.0f);
    scene_.add(sphereMesh_);

    // Cylinder
    auto cylMat = Material::standard();
    cylMat->setColor(Color(0.3f, 0.8f, 0.3f));
    cylMat->setRoughness(0.5f);
    auto cyl = std::make_shared<Mesh>(Geometry::cylinder(0.4f, 0.4f, 1.2f, 24), cylMat);
    cyl->setName("Cylinder");
    cyl->setPosition(3.0f, 0.6f, 0.0f);
    scene_.add(cyl);

    // Gold sphere (PBR)
    auto goldMat = Material::standard();
    goldMat->setColor(Color(1.0f, 0.765f, 0.336f));
    goldMat->setMetallic(1.0f);
    goldMat->setRoughness(0.1f);
    auto gold = std::make_shared<Mesh>(Geometry::sphere(0.5f, 32, 16), goldMat);
    gold->setName("Gold");
    gold->setPosition(-1.5f, 0.5f, 3.0f);
    scene_.add(gold);

    // Emissive cube
    auto emMat = Material::standard();
    emMat->setColor(Color(0.1f, 0.1f, 0.1f));
    emMat->setEmissiveColor(Color(1.0f, 0.3f, 0.1f));
    emMat->setEmissiveIntensity(3.0f);
    emissiveCube_ = std::make_shared<Mesh>(Geometry::box(0.4f, 0.4f, 0.4f), emMat);
    emissiveCube_->setName("Emissive");
    emissiveCube_->setPosition(0.0f, 1.5f, 3.0f);
    scene_.add(emissiveCube_);

    // PBR roughness gradient (5 spheres)
    for (int i = 0; i < 5; ++i) {
        auto mat = Material::standard();
        mat->setColor(Color(0.9f, 0.1f, 0.1f));
        mat->setMetallic(0.5f);
        mat->setRoughness(static_cast<float>(i) / 4.0f * 0.9f + 0.05f);
        auto s = std::make_shared<Mesh>(Geometry::sphere(0.3f, 24, 12), mat);
        s->setName("Roughness" + std::to_string(i));
        s->setPosition(-4.0f + static_cast<float>(i) * 2.0f, 0.3f, -5.0f);
        scene_.add(s);
    }

    // Lights
    auto sun = std::make_shared<DirectionalLight>(Color::White, 0.8f);
    sun->setDirection(-1.0f, -1.0f, -0.5f);
    scene_.add(sun);

    auto ambient = std::make_shared<AmbientLight>(Color::White, 0.05f);
    scene_.add(ambient);

    auto red = std::make_shared<PointLight>(Color(1.0f, 0.2f, 0.1f), 2.0f);
    red->setPosition(-2.0f, 2.0f, 2.0f);
    red->setRange(12.0f);
    scene_.add(red);

    auto blue = std::make_shared<PointLight>(Color(0.1f, 0.3f, 1.0f), 2.0f);
    blue->setPosition(2.0f, 2.0f, -2.0f);
    blue->setRange(12.0f);
    scene_.add(blue);

    NOX_LOG_INFO("LevelGeometry::setup() done - {} meshes, {} lights",
                 scene_.meshes().size(), scene_.lights().size());
}

void LevelGeometry::teardown([[maybe_unused]] Engine& engine) {
    scene_ = Scene3D{};
    boxMesh_.reset();
    sphereMesh_.reset();
    emissiveCube_.reset();
    timer_ = 0.0f;
}

void LevelGeometry::update(Engine& engine, float dt) {
    timer_ += dt;
    orbitCtrl_.update(engine.input(), dt);

    if (boxMesh_) { boxMesh_->rotate(0.0f, 60.0f * dt, 0.0f); }
    if (sphereMesh_) { sphereMesh_->rotate(0.0f, 30.0f * dt, 0.0f); }
    if (emissiveCube_) { emissiveCube_->rotate(45.0f * dt, 30.0f * dt, 0.0f); }
}

void LevelGeometry::drawUI() {
#ifdef NOX_HAS_IMGUI
    ImGui::Text("Geometry & PBR showcase");
    ImGui::Text("Orbit: middle mouse + drag");
#endif
}

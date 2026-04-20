// Copyright (c) 2026 NoxFly — AGPL-3.0

#include "levels/LevelAnimation.hpp"

#ifdef NOX_HAS_IMGUI
#include <imgui.h>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cmath>
#include <numbers>

using namespace Nox;

void LevelAnimation::setup(Engine& engine) {
    camera_.setAspect(engine.aspect());
    camera_.setPosition(4.0f, 3.0f, 4.0f);
    camera_.lookAt(0.0f, 1.5f, 0.0f);
    orbitCtrl_.syncFromCamera();

    // Floor
    auto floorMat = Material::standard();
    floorMat->setColor(Color(0.3f, 0.3f, 0.35f));
    floorMat->setRoughness(0.9f);
    auto floor = std::make_shared<Mesh>(Geometry::plane(15.0f, 15.0f), floorMat);
    scene_.add(floor);

    // Build the procedural segmented arm
    buildProceduralArm();

    // Create skeleton and animations
    skeleton_ = std::make_shared<Skeleton>();
    for (int i = 0; i < ArmSegments; ++i) {
        Joint joint;
        joint.name = "Joint" + std::to_string(i);
        joint.parentIndex = (i == 0) ? -1 : (i - 1);
        joint.inverseBindMatrix = Math::Mat4(1.0f);
        joint.localTransform = glm::translate(Math::Mat4(1.0f), Math::Vec3(0.0f, 0.8f, 0.0f));
        skeleton_->addJoint(std::move(joint));
    }

    buildBounceAnimation();
    buildWaveAnimation();

    player_.setSkeleton(skeleton_);
    player_.setClip(bounceClip_);
    player_.setLooping(true);
    player_.play();

    // Lights
    auto sun = std::make_shared<DirectionalLight>(Color::White, 0.8f);
    sun->setDirection(-1.0f, -1.0f, -0.5f);
    scene_.add(sun);

    auto ambient = std::make_shared<AmbientLight>(Color::White, 0.1f);
    scene_.add(ambient);
}

void LevelAnimation::buildProceduralArm() {
    for (int i = 0; i < ArmSegments; ++i) {
        auto mat = Material::standard();
        float t = static_cast<float>(i) / static_cast<float>(ArmSegments - 1);
        mat->setColor(Color(0.2f + 0.6f * t, 0.4f, 0.8f - 0.6f * t));
        mat->setRoughness(0.3f);
        mat->setMetallic(0.5f);

        auto mesh = std::make_shared<Mesh>(Geometry::box(0.3f, 0.7f, 0.3f), mat);
        mesh->setName("Arm" + std::to_string(i));
        mesh->setPosition(0.0f, 0.35f + static_cast<float>(i) * 0.8f, 0.0f);
        scene_.add(mesh);
        armSegments_[i] = mesh;
    }
}

void LevelAnimation::buildBounceAnimation() {
    bounceClip_ = std::make_shared<AnimationClip>("Bounce");
    bounceClip_->setDuration(2.0f);

    for (int i = 0; i < ArmSegments; ++i) {
        AnimationChannel ch;
        ch.jointIndex = i;
        ch.interpolation = Interpolation::Linear;

        float phase = static_cast<float>(i) * 0.3f;

        // Oscillating Y position
        ch.positionKeys.push_back({ 0.0f, Math::Vec3(0.0f, 0.0f, 0.0f) });
        ch.positionKeys.push_back({ 0.5f + phase, Math::Vec3(0.0f, 0.3f, 0.0f) });
        ch.positionKeys.push_back({ 1.0f + phase, Math::Vec3(0.0f, 0.0f, 0.0f) });
        ch.positionKeys.push_back({ 2.0f, Math::Vec3(0.0f, 0.0f, 0.0f) });

        // Identity rotation
        ch.rotationKeys.push_back({ 0.0f, Math::Quat(1, 0, 0, 0) });

        // Uniform scale
        ch.scaleKeys.push_back({ 0.0f, Math::Vec3(1.0f) });

        bounceClip_->addChannel(std::move(ch));
    }
}

void LevelAnimation::buildWaveAnimation() {
    waveClip_ = std::make_shared<AnimationClip>("Wave");
    waveClip_->setDuration(3.0f);

    for (int i = 0; i < ArmSegments; ++i) {
        AnimationChannel ch;
        ch.jointIndex = i;
        ch.interpolation = Interpolation::Linear;

        float angle = 15.0f * static_cast<float>(i + 1);
        float radians = glm::radians(angle);

        // No position change
        ch.positionKeys.push_back({ 0.0f, Math::Vec3(0.0f) });

        // Swing rotation around Z axis
        ch.rotationKeys.push_back({ 0.0f, Math::Quat(1, 0, 0, 0) });
        ch.rotationKeys.push_back({ 0.75f, glm::angleAxis(radians, Math::Vec3(0, 0, 1)) });
        ch.rotationKeys.push_back({ 1.5f, Math::Quat(1, 0, 0, 0) });
        ch.rotationKeys.push_back({ 2.25f, glm::angleAxis(-radians, Math::Vec3(0, 0, 1)) });
        ch.rotationKeys.push_back({ 3.0f, Math::Quat(1, 0, 0, 0) });

        ch.scaleKeys.push_back({ 0.0f, Math::Vec3(1.0f) });

        waveClip_->addChannel(std::move(ch));
    }
}

void LevelAnimation::teardown([[maybe_unused]] Engine& engine) {
    scene_ = Scene3D{};
    for (auto& seg : armSegments_) { seg.reset(); }
    player_.stop();
}

void LevelAnimation::update(Engine& engine, float dt) {
    orbitCtrl_.update(engine.input(), dt);

    player_.setSpeed(speed_);
    if (playing_) {
        player_.update(dt);
    }

    // Apply bone matrices as transforms to arm segments
    const auto& bones = player_.boneMatrices();
    Math::Mat4 parentWorld(1.0f);

    for (int i = 0; i < ArmSegments && i < static_cast<int>(bones.size()); ++i) {
        if (!armSegments_[i]) { continue; }

        // The bone matrix encodes the full accumulated transform.
        // We apply it as the segment's world position/rotation.
        Math::Mat4 boneWorld = bones[static_cast<size_t>(i)];
        Math::Vec3 basePos(0.0f, 0.35f + static_cast<float>(i) * 0.8f, 0.0f);
        Math::Vec3 boneTrans(boneWorld[3]);
        armSegments_[i]->setPosition(basePos.x + boneTrans.x,
                                     basePos.y + boneTrans.y,
                                     basePos.z + boneTrans.z);
    }
}

void LevelAnimation::drawUI() {
#ifdef NOX_HAS_IMGUI
    ImGui::Text("Skeletal Animation Demo");
    ImGui::Separator();

    if (ImGui::Checkbox("Playing", &playing_)) {
        if (playing_) { player_.play(); }
        else { player_.pause(); }
    }

    ImGui::SliderFloat("Speed", &speed_, 0.0f, 3.0f);

    const char* clips[] = { "Bounce", "Wave" };
    if (ImGui::Combo("Animation", &currentClip_, clips, 2)) {
        if (currentClip_ == 0) {
            player_.crossFadeTo(bounceClip_, 0.5f);
        }
        else {
            player_.crossFadeTo(waveClip_, 0.5f);
        }
    }

    ImGui::Text("Time: %.2f / %.2f",
                static_cast<double>(player_.currentTime()),
                currentClip_ == 0 ? 2.0 : 3.0);
#endif
}

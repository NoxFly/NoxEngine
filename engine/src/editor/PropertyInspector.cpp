// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/editor/PropertyInspector.hpp>
#include <NoxEngine/scene/Light.hpp>
#include <NoxEngine/scene/Mesh.hpp>

#ifdef NOX_HAS_IMGUI
#include <imgui.h>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Nox {

    void PropertyInspector::draw([[maybe_unused]] SceneObject* object) {
#ifdef NOX_HAS_IMGUI
        ImGui::SetNextWindowPos(ImVec2(270, 250), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(320, 450), ImGuiCond_FirstUseEver);

        if (ImGui::Begin("Property Inspector")) {
            if (!object) {
                ImGui::TextDisabled("No object selected");
                ImGui::End();
                return;
            }

            // Name
            char nameBuf[256]{};
            std::string currentName = object->name();
            std::copy(currentName.begin(),
                      currentName.begin() + std::min(currentName.size(), size_t{255}),
                      nameBuf);
            if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf))) {
                object->setName(nameBuf);
            }

            ImGui::Separator();

            // Transform
            drawTransform(*object);

            ImGui::Separator();

            // Type-specific properties (using type tags — no RTTI)
            if (object->isMesh()) {
                drawMeshProperties(static_cast<Mesh&>(*object));
            }
            else if (object->isLight()) {
                drawLightProperties(static_cast<Light&>(*object));
            }
        }
        ImGui::End();
#endif
    }

    void PropertyInspector::drawTransform([[maybe_unused]] SceneObject& object) {
#ifdef NOX_HAS_IMGUI
        if (!ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            return;
        }

        auto& transform = object.transform();

        // Position
        Math::Vec3 pos = transform.position();
        if (ImGui::DragFloat3("Position", &pos.x, 0.05f)) {
            object.setPosition(pos);
        }

        // Rotation (display as Euler degrees)
        Math::Quat rot = transform.rotation();
        Math::Vec3 euler = glm::degrees(glm::eulerAngles(rot));
        if (ImGui::DragFloat3("Rotation", &euler.x, 0.5f)) {
            Math::Quat newRot = Math::Quat(glm::radians(euler));
            transform.setRotation(newRot);
        }

        // Scale
        Math::Vec3 scale = transform.scale();
        if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.01f, 100.0f)) {
            transform.setScale(scale);
        }
#endif
    }

    void PropertyInspector::drawMeshProperties([[maybe_unused]] Mesh& mesh) {
#ifdef NOX_HAS_IMGUI
        if (!ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
            return;
        }

        auto& mat = *mesh.material();

        // Color
        Color col = mat.color();
        float rgba[4] = { col.r, col.g, col.b, col.a };
        if (ImGui::ColorEdit4("Color", rgba)) {
            mat.setColor(Color(rgba[0], rgba[1], rgba[2], rgba[3]));
        }

        if (mat.isLit()) {
            // Roughness
            float roughness = mat.roughness();
            if (ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f)) {
                mat.setRoughness(roughness);
            }

            // Metallic
            float metallic = mat.metallic();
            if (ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f)) {
                mat.setMetallic(metallic);
            }

            // Emissive
            Color ec = mat.emissiveColor();
            float emRgba[4] = { ec.r, ec.g, ec.b, ec.a };
            if (ImGui::ColorEdit3("Emissive Color", emRgba)) {
                mat.setEmissiveColor(Color(emRgba[0], emRgba[1], emRgba[2], 1.0f));
            }

            float emIntensity = mat.emissiveIntensity();
            if (ImGui::DragFloat("Emissive Intensity", &emIntensity, 0.01f, 0.0f, 50.0f)) {
                mat.setEmissiveIntensity(emIntensity);
            }
        }

        // Geometry info
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Geometry Info")) {
            const auto& geom = *mesh.geometry();
            ImGui::Text("Vertices: %zu", geom.vertices().size());
            ImGui::Text("Indices:  %zu", geom.indices().size());
            const auto& aabb = geom.boundingBox();
            ImGui::Text("AABB min: (%.2f, %.2f, %.2f)", aabb.min.x, aabb.min.y, aabb.min.z);
            ImGui::Text("AABB max: (%.2f, %.2f, %.2f)", aabb.max.x, aabb.max.y, aabb.max.z);
        }
#endif
    }

    void PropertyInspector::drawLightProperties(Light& light) {
        switch (light.objectType()) {
            case SceneObjectType::DirectionalLight:
                drawDirectionalLightProperties(static_cast<DirectionalLight&>(light));
                break;
            case SceneObjectType::PointLight:
                drawPointLightProperties(static_cast<PointLight&>(light));
                break;
            case SceneObjectType::AmbientLight:
                drawAmbientLightProperties(static_cast<AmbientLight&>(light));
                break;
            default:
                break;
        }
    }

    void PropertyInspector::drawDirectionalLightProperties([[maybe_unused]] DirectionalLight& light) {
#ifdef NOX_HAS_IMGUI
        if (!ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen)) {
            return;
        }

        Color col = light.color();
        float rgb[3] = { col.r, col.g, col.b };
        if (ImGui::ColorEdit3("Light Color", rgb)) {
            light.setColor(Color(rgb[0], rgb[1], rgb[2]));
        }

        float intensity = light.intensity();
        if (ImGui::DragFloat("Intensity", &intensity, 0.01f, 0.0f, 20.0f)) {
            light.setIntensity(intensity);
        }

        Math::Vec3 dir = light.direction();
        if (ImGui::DragFloat3("Direction", &dir.x, 0.01f, -1.0f, 1.0f)) {
            light.setDirection(dir);
        }
#endif
    }

    void PropertyInspector::drawPointLightProperties([[maybe_unused]] PointLight& light) {
#ifdef NOX_HAS_IMGUI
        if (!ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_DefaultOpen)) {
            return;
        }

        Color col = light.color();
        float rgb[3] = { col.r, col.g, col.b };
        if (ImGui::ColorEdit3("Light Color", rgb)) {
            light.setColor(Color(rgb[0], rgb[1], rgb[2]));
        }

        float intensity = light.intensity();
        if (ImGui::DragFloat("Intensity", &intensity, 0.01f, 0.0f, 20.0f)) {
            light.setIntensity(intensity);
        }

        float range = light.range();
        if (ImGui::DragFloat("Range", &range, 0.1f, 0.1f, 100.0f)) {
            light.setRange(range);
        }
#endif
    }

    void PropertyInspector::drawAmbientLightProperties([[maybe_unused]] AmbientLight& light) {
#ifdef NOX_HAS_IMGUI
        if (!ImGui::CollapsingHeader("Ambient Light", ImGuiTreeNodeFlags_DefaultOpen)) {
            return;
        }

        Color col = light.color();
        float rgb[3] = { col.r, col.g, col.b };
        if (ImGui::ColorEdit3("Light Color", rgb)) {
            light.setColor(Color(rgb[0], rgb[1], rgb[2]));
        }

        float intensity = light.intensity();
        if (ImGui::DragFloat("Intensity", &intensity, 0.01f, 0.0f, 5.0f)) {
            light.setIntensity(intensity);
        }
#endif
    }

} // namespace Nox

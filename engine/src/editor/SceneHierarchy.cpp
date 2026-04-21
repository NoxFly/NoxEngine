// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/editor/SceneHierarchy.hpp>
#include <NoxEngine/scene/Light.hpp>
#include <NoxEngine/scene/Mesh.hpp>

#ifdef NOX_HAS_IMGUI
#include <imgui.h>
#endif

namespace Nox {

    void SceneHierarchy::draw([[maybe_unused]] Scene3D& scene) {
#ifdef NOX_HAS_IMGUI
        ImGui::SetNextWindowPos(ImVec2(10, 250), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(250, 350), ImGuiCond_FirstUseEver);

        if (ImGui::Begin("Scene Hierarchy")) {
            const auto& objects = scene.objects();

            if (objects.empty()) {
                ImGui::TextDisabled("(empty scene)");
            }

            for (const auto& obj : objects) {
                drawObjectNode(*obj);
            }

            // Click on empty space to deselect
            if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)
                && !ImGui::IsAnyItemHovered()) {
                selected_ = nullptr;
            }
        }
        ImGui::End();
#endif
    }

    void SceneHierarchy::drawObjectNode([[maybe_unused]] SceneObject& object) {
#ifdef NOX_HAS_IMGUI
        bool isSelected = (selected_ == &object);

        // Determine icon based on type
        const char* icon = "  ";
        switch (object.objectType()) {
            case SceneObjectType::Mesh:             icon = "[M] "; break;
            case SceneObjectType::DirectionalLight: icon = "[D] "; break;
            case SceneObjectType::PointLight:       icon = "[P] "; break;
            case SceneObjectType::AmbientLight:     icon = "[A] "; break;
            default: break;
        }

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf
                                 | ImGuiTreeNodeFlags_NoTreePushOnOpen
                                 | ImGuiTreeNodeFlags_SpanAvailWidth;

        if (isSelected) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        std::string label = std::string(icon) + object.name();
        ImGui::TreeNodeEx(static_cast<void*>(&object), flags, "%s", label.c_str());

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
            selected_ = &object;
        }
#endif
    }

} // namespace Nox

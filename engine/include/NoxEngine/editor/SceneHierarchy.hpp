// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/scene/Scene3D.hpp>

namespace Nox {

    /// ImGui panel that displays the scene graph as a tree view.
    /// Clicking a node selects it for inspection via PropertyInspector.
    class SceneHierarchy {
    public:
        SceneHierarchy() = default;

        /// Draw the hierarchy panel. Call inside an ImGui frame.
        void draw(Scene3D& scene);

        /// Currently selected object (nullptr if none).
        [[nodiscard]] SceneObject* selected() const { return selected_; }

        /// Programmatically select an object.
        void setSelected(SceneObject* obj) { selected_ = obj; }

        /// Clear selection.
        void clearSelection() { selected_ = nullptr; }

    private:
        void drawObjectNode(SceneObject& object);

        SceneObject* selected_ = nullptr;
    };

} // namespace Nox

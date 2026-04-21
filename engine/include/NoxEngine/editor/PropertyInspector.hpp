// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/scene/SceneObject.hpp>

namespace Nox {

    class Mesh;
    class Light;
    class DirectionalLight;
    class PointLight;
    class AmbientLight;

    /// ImGui panel that displays and edits properties of a selected SceneObject.
    /// Supports Transform, Material parameters, and Light-specific fields.
    class PropertyInspector {
    public:
        PropertyInspector() = default;

        /// Draw the inspector panel for the given object. Call inside an ImGui frame.
        void draw(SceneObject* object);

    private:
        void drawTransform(SceneObject& object);
        void drawMeshProperties(Mesh& mesh);
        void drawLightProperties(Light& light);
        void drawDirectionalLightProperties(DirectionalLight& light);
        void drawPointLightProperties(PointLight& light);
        void drawAmbientLightProperties(AmbientLight& light);
    };

} // namespace Nox

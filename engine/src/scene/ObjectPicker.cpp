// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/scene/ObjectPicker.hpp>
#include <NoxEngine/scene/Mesh.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Nox {

    Math::Ray ObjectPicker::screenToRay(const PerspectiveCamera& camera,
                                         const Math::Vec2& screenPos,
                                         int viewportWidth, int viewportHeight) {
        // Convert screen position to normalized device coordinates
        float ndcX = (2.0f * screenPos.x) / static_cast<float>(viewportWidth) - 1.0f;
        float ndcY = 1.0f - (2.0f * screenPos.y) / static_cast<float>(viewportHeight);

        // Unproject near and far points
        Math::Mat4 invViewProj = glm::inverse(camera.projectionMatrix() * camera.viewMatrix());

        Math::Vec4 nearClip = invViewProj * Math::Vec4(ndcX, ndcY, -1.0f, 1.0f);
        Math::Vec4 farClip  = invViewProj * Math::Vec4(ndcX, ndcY,  1.0f, 1.0f);

        Math::Vec3 nearWorld = Math::Vec3(nearClip) / nearClip.w;
        Math::Vec3 farWorld  = Math::Vec3(farClip) / farClip.w;

        Math::Vec3 direction = glm::normalize(farWorld - nearWorld);

        return Math::Ray{ .origin = camera.position(), .direction = direction };
    }

    std::shared_ptr<Mesh> ObjectPicker::pick(const Scene3D& scene,
                                              const PerspectiveCamera& camera,
                                              const Math::Vec2& screenPos,
                                              int viewportWidth, int viewportHeight) const {
        Math::Ray ray = screenToRay(camera, screenPos, viewportWidth, viewportHeight);

        float closestDist = std::numeric_limits<float>::max();
        std::shared_ptr<Mesh> closestMesh;

        for (const auto& mesh : scene.meshes()) {
            const auto& aabb = mesh->geometry()->boundingBox();

            // Transform AABB to world space (approximate)
            const auto& worldMat = mesh->worldMatrix();
            Math::Vec3 worldCenter = Math::Vec3(worldMat * Math::Vec4(
                (aabb.min + aabb.max) * 0.5f, 1.0f));
            Math::Vec3 halfExtent = (aabb.max - aabb.min) * 0.5f;

            Math::Vec3 scale{
                glm::length(Math::Vec3(worldMat[0])),
                glm::length(Math::Vec3(worldMat[1])),
                glm::length(Math::Vec3(worldMat[2]))
            };
            halfExtent *= scale;

            Math::AABB worldAABB{
                worldCenter - halfExtent,
                worldCenter + halfExtent
            };

            float tMin = 0.0f;
            float tMax = 0.0f;
            if (ray.intersectsAABB(worldAABB, tMin, tMax)) {
                if (tMin < closestDist && tMin >= 0.0f) {
                    closestDist = tMin;
                    closestMesh = mesh;
                }
            }
        }

        return closestMesh;
    }

    std::shared_ptr<Mesh> ObjectPicker::pickFromMouse(const Scene3D& scene,
                                                       const PerspectiveCamera& camera,
                                                       const Input& input,
                                                       int viewportWidth,
                                                       int viewportHeight) const {
        return pick(scene, camera, input.mousePosition(), viewportWidth, viewportHeight);
    }

} // namespace Nox

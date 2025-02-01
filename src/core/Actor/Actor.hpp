/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef ACTOR_HPP
#define ACTOR_HPP

#include <type_traits>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
#include <memory>

#include "core/engine.typedef.hpp"
#include "core/MatricesMVP/Matrices.hpp"
#include "core/Camera/Camera.hpp"
#include "Geometries/Geometry.hpp"
#include "Materials/Material.hpp"
#include "Movable.hpp"


namespace NoxEngine {

    class Scene;

    class Actor: public Movable<true> {
        public:
            static void setObjectsPath(const std::string& objectsPath) noexcept;

            explicit Actor();
            explicit Actor(Geometry* geometry, Material* material);
            // Actor(const Actor& copy);
            // const Actor& operator=(const Actor& copy);
            virtual ~Actor();

            const std::string& getUUID() const noexcept;

            Geometry* getGeometry() noexcept;
            Material* getMaterial() noexcept;

            const V3D& getRotation() const noexcept;

            void setPosition(const V3D& position) noexcept;
            void setRotation(const V3D& rotation) noexcept;

            void setPosition(const float x, const float y, const float z) noexcept;
            void setRotation(const float rx, const float ry, const float rz) noexcept;

            void rotate(const float x, const float y, const float z) noexcept;

            void scale(const float x, const float y, const float z) noexcept;

            void setCullFace(const GLenum cullFace) noexcept;
            void setPolygonMode(const GLenum polygonMode) noexcept;

            void render(Scene* scene, Camera* camera);

        protected:
            static std::string m_objectsPath;

            const std::string m_uuid;

            Geometry* m_geometry;
            Material* m_material;
            GLenum m_cullFace, m_polygonMode;

            V3D m_rotation;
            V3D m_scale;
            bool m_hasToTranslate, m_hasToRotate, m_hasToScale;
    };

}

#endif // ACTOR_HPP
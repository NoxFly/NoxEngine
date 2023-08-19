#ifndef ACTOR_HPP
#define ACTOR_HPP

#include <type_traits>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
#include <memory>

#include "core/engine.typedef.hpp"
#include "Geometries/Geometry.hpp"
#include "Materials/Material.hpp"
#include "core/MatricesMVP/Matrices.hpp"
#include "Movable.hpp"


namespace NoxEngine {

    template <Dimension D>
    class Scene;
    template <Dimension D>
    class Camera;

    template <Dimension D>
    class Actor: public Movable<D> {
        public:
            explicit Actor();
            explicit Actor(Geometry* geometry, Material<D>* material);
            // Actor(const Actor<D>& copy);
            // const Actor<D>& operator=(const Actor<D>& copy);
            virtual ~Actor();

            const std::string& getUUID() const noexcept;

            Geometry* getGeometry() noexcept;
            Material<D>* getMaterial() noexcept;

            const D& getRotation() const noexcept;

            void setPosition(const D& position) noexcept;
            void setRotation(const D& rotation) noexcept;

            void setPosition(const float x, const float y) noexcept requires Is2D<D>;
            void setPosition(const float x, const float y, const float z) noexcept requires Is3D<D>;
            void setRotation(const float rx, const float ry) noexcept requires Is2D<D>;
            void setRotation(const float rx, const float ry, const float rz) noexcept requires Is3D<D>;

            void rotate(const float x, const float y) noexcept requires Is2D<D>;
            void rotate(const float x, const float y, const float z) noexcept requires Is3D<D>;

            void setCullFace(const GLenum cullFace) noexcept;
            void setPolygonMode(const GLenum polygonMode) noexcept;

            void render(Scene<D>* scene, Camera<D>* camera);

        protected:
            const std::string m_uuid;
            const unsigned int m_dimension;
            const bool m_is3D;

            Geometry* m_geometry;
            Material<D>* m_material;
            GLenum m_cullFace, m_polygonMode;

            D m_rotation;
            bool m_hasToTranslate, m_hasToRotate;
    };

}

#include "Actor.inl"

#endif // ACTOR_HPP
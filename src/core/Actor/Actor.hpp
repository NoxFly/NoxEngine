#ifndef ACTOR_HPP
#define ACTOR_HPP

#include <type_traits>
#include <glm/glm.hpp>

#include "engine.typedef.hpp"
#include "Geometry.hpp"
#include "Material.hpp"
#include "Matrices.hpp"
#include "Movable.hpp"



#ifndef BUFFER_OFFSET
    #define BUFFER_OFFSET(offset) ((char*)NULL + (offset))
#endif


namespace NoxEngine {

    template <Dimension D>
    class Actor: public Movable<D> {
        public:
            Actor();
            Actor(const Geometry& geometry, const Material& material);
            // Actor(const Actor<D>& copy);
            // const Actor<D>& operator=(const Actor<D>& copy);
            virtual ~Actor();

            const std::string& getUUID() const;

            Geometry* getGeometry();
            Material* getMaterial();

            const D& getRotation() const;

            void setPosition(const D& position) noexcept;
            void setRotation(const D& rotation) noexcept;

            void setPosition(const float x, const float y) noexcept requires Is2D<D>;
            void setPosition(const float x, const float y, const float z) noexcept requires Is3D<D>;
            void setRotation(const float rx, const float ry) noexcept requires Is2D<D>;
            void setRotation(const float rx, const float ry, const float rz) noexcept requires Is3D<D>;

            void setCullFace(const GLenum cullFace) noexcept;
            void setPolygonMode(const GLenum polygonMode) noexcept;

            bool load();

            void render(Matrices<D>* mvp);

        protected:
            const std::string m_uuid;
            const uint m_dimension;
            const bool m_is3D;

            Geometry m_geometry;
            Material m_material;
            GLuint m_VBO, m_VAO;
            GLenum m_cullFace, m_polygonMode;
            size_t m_vertexNumber;

            D m_rotation;
            bool m_hasToTranslate, m_hasToRotate;

        private:
            void deleteVBO() noexcept;
            void deleteVAO() noexcept;

            bool m_isLoaded;
    };

}

#include "Actor.inl"

#endif // ACTOR_HPP
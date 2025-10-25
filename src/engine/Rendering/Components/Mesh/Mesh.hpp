/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef MESH_HPP
#define MESH_HPP

#include <string>
#include <GL/glew.h>

#include "engine/core/Actor/Drawable.hpp"
#include "engine/Rendering/Geometries/BufferGeometry.hpp"
#include "engine/Rendering/Materials/Material.hpp"
#include "engine/core/MatricesMVP/Matrices.hpp"
#include "engine/core/Scene/Scene.hpp"

namespace NoxEngine {

	class Mesh: public Drawable {
		public:
			static std::shared_ptr<Mesh> create(std::shared_ptr<BufferGeometry> geom, std::shared_ptr<Material> mat);

			~Mesh() = default;

			Mesh(const Mesh&) = delete;
    		Mesh& operator=(const Mesh&) = delete;

			std::shared_ptr<BufferGeometry> getGeometry() const noexcept { return m_geometry; }
    		std::shared_ptr<Material> getMaterial() const noexcept { return m_material; }

			void setCullFace(const GLenum cullFace) noexcept;
            void setPolygonMode(const GLenum polygonMode) noexcept;

			void draw(Scene* scene, Matrices& mvp);

		protected:
			explicit Mesh(std::shared_ptr<BufferGeometry> geom, std::shared_ptr<Material> mat);

			std::shared_ptr<BufferGeometry> m_geometry;
    		std::shared_ptr<Material> m_material;

            GLenum m_cullFace { GL_BACK };
			GLenum m_polygonMode { GL_FILL };
	};

}

#endif // MESH_HPP
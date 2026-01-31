/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef MESH_HPP
#define MESH_HPP

#include <string>
#include <GL/glew.h>

#include "NoxEngine/Actor/Drawable.hpp"
#include "NoxEngine/Camera/Matrices.hpp"
#include "NoxEngine/Geometry/BufferGeometry.hpp"
#include "NoxEngine/Material/Material.hpp"
#include "NoxEngine/Rendering/Scene.hpp"

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

			void draw(Scene* scene, Matrices& mvp, const V3D& cameraPosition);

		protected:
			explicit Mesh(std::shared_ptr<BufferGeometry> geom, std::shared_ptr<Material> mat);

			std::shared_ptr<BufferGeometry> m_geometry;
    		std::shared_ptr<Material> m_material;

            GLenum m_cullFace { GL_BACK };
			GLenum m_polygonMode { GL_FILL };
	};

}

#endif // MESH_HPP
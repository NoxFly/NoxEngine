/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef MESH3D_HPP
#define MESH3D_HPP

#include <string>
#include <GL/glew.h>

#include "engine/core/Actor/Drawable.hpp"
#include "engine/Rendering/Geometries/Geometry.hpp"
#include "engine/Rendering/Materials/Material.hpp"
#include "engine/core/MatricesMVP/Matrices.hpp"
#include "engine/core/Scene/Scene.hpp"

namespace NoxEngine {

	class Mesh3D: public Drawable {
		public:
			explicit Mesh3D();
			explicit Mesh3D(Geometry* geometry, Material* material);
			~Mesh3D() = default;

			bool loadFromFile(const std::string& filepath);

			Geometry* getGeometry() noexcept;
            Material* getMaterial() noexcept;

			void setCullFace(const GLenum cullFace) noexcept;
            void setPolygonMode(const GLenum polygonMode) noexcept;

			void draw(Scene* scene, Matrices& mvp);

		protected:
			Geometry* m_geometry;
            Material* m_material;
            GLenum m_cullFace, m_polygonMode;
	};

}

#endif // MESH3D_HPP
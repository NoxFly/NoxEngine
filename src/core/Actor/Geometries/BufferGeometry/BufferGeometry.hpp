/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef BUFFER_GEOMETRY_HPP
#define BUFFER_GEOMETRY_HPP

#include <vector>
#include <string>
#include <cstring>
#include <map>

#include "core/engine.typedef.hpp"
#include "core/Actor/Geometries/Geometry.hpp"

namespace NoxEngine {

	struct GeometryIndexedData {
		std::vector<V3D> vertices;
		std::vector<V2D> uvs;
		std::vector<V3D> normals;
		std::vector<V3D> colors;

		std::vector<V4D> indexes;
	};

	struct PackedVertex {
		V3D position;
		V2D uv;
		V3D normal;
		V3D color;

		bool operator < (const PackedVertex that) const {
			return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0;
		};
	};

	class BufferGeometry: public Geometry {
		public:
			explicit BufferGeometry();
			~BufferGeometry();

			bool loadFromData(const GeometryData& data);

		protected:
			bool load(const GeometryData& data);

			void indexData(
				const GeometryIndexedData& data,
				GeometryData& out
			);

			bool getSimilarVertexIndex(
				PackedVertex& packed,
				std::map<PackedVertex, GLushort>& vertexToOutIndex,
				GLushort& result
			);
	};

}

#endif // BUFFER_GEOMETRY_HPP
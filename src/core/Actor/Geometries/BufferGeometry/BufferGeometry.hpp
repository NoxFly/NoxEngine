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

	template <Dimension D>
	struct GeometryIndexedData {
		std::vector<D> vertices;
		std::vector<V2D> uvs;
		std::vector<D> normals;
		std::vector<V3D> colors;

		std::vector<V4D> indexes;
	};

	template <Dimension D>
	struct PackedVertex {
		D position;
		V2D uv;
		D normal;
		V3D color;

		bool operator < (const PackedVertex that) const {
			return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0;
		};
	};

	template <Dimension D>
	class BufferGeometry: public Geometry {
		public:
			explicit BufferGeometry();
			~BufferGeometry();

			bool loadFromData(const GeometryData& data);

		protected:
			bool load(const GeometryData& data);

			void indexData(
				const GeometryIndexedData<V3D>& data,
				GeometryData& out
			) requires Is3D<D>;

			void indexData(
				const GeometryIndexedData<V2D>& data,
				GeometryData& out
			) requires Is2D<D>;

			bool getSimilarVertexIndex(
				PackedVertex<D>& packed,
				std::map<PackedVertex<D>, GLushort>& vertexToOutIndex,
				GLushort& result
			);
	};

}

#include "BufferGeometry.inl"

#endif // BUFFER_GEOMETRY_HPP
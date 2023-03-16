#ifndef BUFFER_GEOMETRY_HPP
#define BUFFER_GEOMETRY_HPP

#include <vector>
#include <string>

#include "core/Actor/Geometries/Geometry.hpp"

namespace NoxEngine {

	class BufferGeometry : public Geometry {
		public:
			explicit BufferGeometry();
			~BufferGeometry();

			bool loadFromData(const GeometryData& data);
			bool loadFromFile(const std::string& filepath);

		protected:
			bool load();
	};

}

#endif // BUFFER_GEOMETRY_HPP
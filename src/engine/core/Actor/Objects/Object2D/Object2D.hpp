#ifndef OBJECT_2D_HPP
#define OBJECT_2D_HPP

#include <memory>

#include "core/engine.typedef.hpp"
#include "core/Actor/Actor.hpp"
#include "core/Actor/Geometries/Geometry.hpp"
#include "core/Actor/Materials/Material.hpp"

namespace NoxEngine {

	class Object2D: public Actor<V2D> {

		public:
			explicit Object2D();
			explicit Object2D(const Geometry& geometry, const Material& material);
			~Object2D();
	};

}

#endif // OBJECT_2D_HPP
#ifndef OBJECT_2D_HPP
#define OBJECT_2D_HPP

#include "engine.typedef.hpp"
#include "Actor.hpp"
#include "Geometry.hpp"
#include "Material.hpp"

namespace NoxEngine {

	class Object2D: public Actor<V2D> {

		public:
			explicit Object2D();
			explicit Object2D(const Geometry& geometry, const Material& material);
			~Object2D();
	};

}

#endif // OBJECT_2D_HPP
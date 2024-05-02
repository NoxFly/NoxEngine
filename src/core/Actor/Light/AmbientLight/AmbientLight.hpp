#ifndef AMBIENT_LIGHT_HPP
#define AMBIENT_LIGHT_HPP

#include "core/Actor/Light/Light.hpp"
#include "utils/Color.hpp"

namespace NoxEngine {

	class AmbientLight: public Light {
		public:
			explicit AmbientLight(const Color color, const float intensity);
			~AmbientLight();
	};

}

#endif // AMBIENT_LIGHT_HPP
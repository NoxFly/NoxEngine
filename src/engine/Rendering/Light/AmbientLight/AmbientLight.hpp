/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef AMBIENT_LIGHT_HPP
#define AMBIENT_LIGHT_HPP

#include "engine/Rendering/Light/Light.hpp"
#include "utils/Color.hpp"

namespace NoxEngine {

	class AmbientLight: public Light {
		public:
			explicit AmbientLight(const Color color, const float intensity);
			~AmbientLight() = default;
	};

}

#endif // AMBIENT_LIGHT_HPP
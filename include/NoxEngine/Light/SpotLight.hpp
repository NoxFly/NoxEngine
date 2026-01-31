/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef SPOT_LIGHT_HPP
#define SPOT_LIGHT_HPP

#include "NoxEngine/Light/Light.hpp"
#include "NoxEngine/utils/Color.hpp"

namespace NoxEngine {

	class SpotLight: public Light, public Movable<true> {
		public:
			explicit SpotLight(const Color color = Color(255, 255, 255), const float intensity = 1.0f);
			~SpotLight() = default;

		protected:
			std::string name {"SpotLight"};
	};

}

#endif // SPOT_LIGHT_HPP
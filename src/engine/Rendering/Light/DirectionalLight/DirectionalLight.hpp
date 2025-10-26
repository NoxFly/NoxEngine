/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef DIRECTIONAL_LIGHT_HPP
#define DIRECTIONAL_LIGHT_HPP

#include "engine/Rendering/Light/Light.hpp"
#include "utils/Color.hpp"

namespace NoxEngine {

	class DirectionalLight: public Light, public Movable<true> {
		public:
			explicit DirectionalLight(const Color color = Color(255, 255, 255), const float intensity = 1.0f);
			~DirectionalLight() = default;

		protected:
			std::string name {"DirectionalLight"};
	};

}

#endif // DIRECTIONAL_LIGHT_HPP
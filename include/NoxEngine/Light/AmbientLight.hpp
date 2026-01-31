/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef AMBIENT_LIGHT_HPP
#define AMBIENT_LIGHT_HPP

#include "NoxEngine/Light/Light.hpp"
#include "NoxEngine/utils/Color.hpp"

namespace NoxEngine {

	class AmbientLight: public Light {
		public:
			explicit AmbientLight(const Color color = Color(255, 255, 255), const float intensity = 1.0f);
			~AmbientLight() = default;

		protected:
			std::string name {"AmbientLight"};
	};

}

#endif // AMBIENT_LIGHT_HPP
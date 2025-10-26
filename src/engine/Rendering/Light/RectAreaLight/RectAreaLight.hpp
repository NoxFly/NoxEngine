/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef RECTAREA_LIGHT_HPP
#define RECTAREA_LIGHT_HPP

#include "engine/Rendering/Light/Light.hpp"
#include "utils/Color.hpp"

namespace NoxEngine {

	class RectAreaLight: public Light, public Movable<true> {
		public:
			explicit RectAreaLight(const Color color = Color(255, 255, 255), const float intensity = 1.0f);
			~RectAreaLight() = default;

		protected:
			std::string name {"RectAreaLight"};
	};

}

#endif // RECTAREA_LIGHT_HPP
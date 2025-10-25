/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "engine/core/engine.typedef.hpp"
#include "engine/core/Actor/Movable.hpp"
#include "utils/Color.hpp"

namespace NoxEngine {
	
	class Light: public Movable<true> {
		public:
			explicit Light(const Color color, const float intensity);
			virtual ~Light() = default;

			float getIntensity() const noexcept;
			Color getColor() const noexcept;

		protected:
			float m_intensity;
			Color m_color;
	};

}

#endif // LIGHT_HPP
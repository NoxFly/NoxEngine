#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "core/engine.typedef.hpp"
#include "core/Actor/Movable.hpp"
#include "utils/Color.hpp"

namespace NoxEngine {
	
	class Light: public Movable<V3D> {
		public:
			explicit Light(const Color color, const float intensity);
			virtual ~Light();

			float getIntensity() const noexcept;
			Color getColor() const noexcept;

		protected:
			float m_intensity;
			Color m_color;
	};

}

#endif // LIGHT_HPP
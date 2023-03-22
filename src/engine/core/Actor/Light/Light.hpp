#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "core/engine.typedef.hpp"
#include "core/Actor/Movable.hpp"
#include "utils/Color.hpp"

namespace NoxEngine {
	
	class Light: public Movable<V3D> {
		public:
			Light(const Color color, const float intensity);
			virtual ~Light();

			float getIntensity() const;
			Color getColor() const;

		protected:
			float m_intensity;
			Color m_color;
	};

}

#endif // LIGHT_HPP
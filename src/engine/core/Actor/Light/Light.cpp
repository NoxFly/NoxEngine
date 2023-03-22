#include "Light.hpp"

namespace NoxEngine {

	Light::Light(const Color color, const float intensity):
		m_color(color), m_intensity(intensity)
	{

	}

	Light::~Light() {

	}

	float Light::getIntensity() const {
		return m_intensity;
	}

	Color Light::getColor() const {
		return m_color;
	}

}
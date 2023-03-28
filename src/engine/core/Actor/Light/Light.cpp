#include "Light.hpp"

namespace NoxEngine {

	Light::Light(const Color color, const float intensity):
		m_color(color), m_intensity(intensity)
	{

	}

	Light::~Light() {

	}

	float Light::getIntensity() const noexcept {
		return m_intensity;
	}

	Color Light::getColor() const noexcept {
		return m_color;
	}

}
#include "Light.hpp"

namespace NoxEngine {

	Light::Light(const Color color, const float intensity):
		m_intensity(intensity), m_color(color)
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
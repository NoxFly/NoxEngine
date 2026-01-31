/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "NoxEngine/Light/AmbientLight.hpp"


namespace NoxEngine {

	AmbientLight::AmbientLight(const Color color, const float intensity):
		Light(color, intensity)
	{}

}
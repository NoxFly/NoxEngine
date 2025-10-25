/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Sphere.hpp"

#include "engine/Rendering/Geometries/3D/SphereGeometry/SphereGeometry.hpp"
#include "engine/Rendering/Materials/PhongMaterial/PhongMaterial.hpp"


namespace NoxEngine {

	Sphere::Sphere(const float radius):
		Mesh3D(new SphereGeometry(radius), new PhongMaterial())
	{}

}
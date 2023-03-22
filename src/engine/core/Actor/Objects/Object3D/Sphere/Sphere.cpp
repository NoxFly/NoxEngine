#include "Sphere.hpp"

#include "core/Actor/Materials/PhongMaterial/PhongMaterial.hpp"
#include "core/Actor/Geometries/3D/SphereGeometry/SphereGeometry.hpp"

namespace NoxEngine {

	Sphere::Sphere(const float radius):
		Object3D(SphereGeometry(radius), PhongMaterial())
	{

	}

	Sphere::~Sphere() {

	}

}
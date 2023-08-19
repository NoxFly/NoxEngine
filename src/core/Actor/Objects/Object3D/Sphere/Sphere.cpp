#include "Sphere.hpp"

#include "core/Actor/Geometries/3D/SphereGeometry/SphereGeometry.hpp"
#include "core/Actor/Materials/3D/PhongMaterial/PhongMaterial.hpp"


namespace NoxEngine {

	Sphere::Sphere(const float radius):
		Object3D(new SphereGeometry(radius), new PhongMaterial())
	{}

	Sphere::~Sphere() {}

}
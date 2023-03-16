#include "OrthographicCamera.hpp"


namespace NoxEngine {

	OrthographicCamera::OrthographicCamera(double fov, double aspect, double near, double far):
		Camera3D(fov, aspect, near, far),
		ortho(glm::ortho(-1, 1, 1, -1))
	{

	}

	OrthographicCamera::~OrthographicCamera() {

	}

}
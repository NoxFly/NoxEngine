#include "OrthographicCamera.hpp"


namespace NoxEngine {

	OrthographicCamera::OrthographicCamera(float left, float right, float top, float bottom):
		OrthographicCamera(left, right, top, bottom, 0.1f, 1000.f)
	{

	}

	OrthographicCamera::OrthographicCamera(float left, float right, float top, float bottom, float near, float far):
		Camera3D(left, right, top, bottom, near, far)
	{

	}

	OrthographicCamera::~OrthographicCamera() {

	}

	void OrthographicCamera::move(const V3D& offset, unsigned int duration) {

	}

	void OrthographicCamera::moveTo(const V3D& position, unsigned int duration) {

	}

}
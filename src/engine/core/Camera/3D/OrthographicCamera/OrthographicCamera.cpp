#include "OrthographicCamera.hpp"


namespace NoxEngine {

	OrthographicCamera::OrthographicCamera(const float left, const float right, const float top, const float bottom):
		OrthographicCamera(left, right, top, bottom, 0.1f, 1000.f)
	{

	}

	OrthographicCamera::OrthographicCamera(const float left, const float right, const float top, const float bottom, const float near, const float far):
		Camera3D(left, right, top, bottom, near, far)
	{

	}

	OrthographicCamera::~OrthographicCamera() {

	}

	void OrthographicCamera::move(const V3D& offset, const unsigned int duration) {

	}

	void OrthographicCamera::moveTo(const V3D& position, const unsigned int duration) {

	}

}
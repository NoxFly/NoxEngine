/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef CAMERACONTROL_HPP
#define CAMERACONTROL_HPP

#include "core/engine.typedef.hpp"
#include "core/Camera/ICamera.hpp"
#include "core/Renderer/Input/Input.hpp"

namespace NoxEngine {

	template <typename Cam, typename = std::enable_if_t<std::is_base_of_v<Camera<typename Cam::Type>, Cam>>>
	class CameraControl {
		public:
			virtual ~CameraControl() = default;
			virtual void update(Cam& camera, const Input& input, const float deltaTime) = 0;
	};

}

#endif // CAMERACONTROL_HPP
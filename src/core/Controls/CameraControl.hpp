/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef CAMERACONTROL_HPP
#define CAMERACONTROL_HPP

#include "core/engine.typedef.hpp"
#include "core/Camera/Camera.hpp"
#include "core/Renderer/Input/Input.hpp"
#include "core/Renderer/Renderer.hpp"

namespace NoxEngine {

	template <typename C, typename = std::enable_if_t<std::is_base_of_v<Camera, C>>>
	class CameraControl {
		public:
			virtual ~CameraControl() {}
			virtual void update() = 0;
	};

}

#endif // CAMERACONTROL_HPP
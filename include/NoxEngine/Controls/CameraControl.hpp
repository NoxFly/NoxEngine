/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef CAMERACONTROL_HPP
#define CAMERACONTROL_HPP

#include "NoxEngine/engine.typedef.hpp"
#include "NoxEngine/Camera/Camera.hpp"
#include "NoxEngine/Rendering/Input.hpp"
#include "NoxEngine/Rendering/Renderer.hpp"

namespace NoxEngine {

	template <typename C, typename = std::enable_if_t<std::is_base_of_v<Camera, C>>>
	class CameraControl {
		public:
			virtual ~CameraControl() {}
			virtual void update() = 0;
	};

}

#endif // CAMERACONTROL_HPP
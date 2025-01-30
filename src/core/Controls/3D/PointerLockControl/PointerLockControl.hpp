/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef POINTERLOCKCONTROL_HPP
#define POINTERLOCKCONTROL_HPP

#include "core/Controls/CameraControl.hpp"
#include "core/Camera/3D/PerspectiveCamera/PerspectiveCamera.hpp"
#include "core/Renderer/Renderer.hpp"

namespace NoxEngine {

	class PointerLockControl: public CameraControl<V3D, PerspectiveCamera> {
		public:
			explicit PointerLockControl();
			
			void update(Renderer& renderer, Scene<V3D>& scene, PerspectiveCamera& camera) override;
	};

}

#endif // POINTERLOCKCONTROL_HPP
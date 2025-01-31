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
			explicit PointerLockControl(Renderer& renderer, PerspectiveCamera& camera);
			
			void update() override;

		private:
			Renderer& m_renderer;
			PerspectiveCamera& m_camera;
			float m_sensitivity, m_speed;
	};

}

#endif // POINTERLOCKCONTROL_HPP
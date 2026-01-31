/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-31
 * @license AGPL-3.0
 */

#ifndef ARCBALL_CONTROL_HPP
#define ARCBALL_CONTROL_HPP

#include "NoxEngine/Controls/CameraControl.hpp"
#include "NoxEngine/Camera/PerspectiveCamera.hpp"
#include "NoxEngine/Rendering/Renderer.hpp"

namespace NoxEngine {

	class ArcballControls: public CameraControl<PerspectiveCamera> {
		public:
			explicit ArcballControls(Renderer& renderer, PerspectiveCamera& camera);

			void update() override;

		private:
			Renderer& m_renderer;
			PerspectiveCamera& m_camera;
	};

}

#endif // ARCBALL_CONTROL_HPP
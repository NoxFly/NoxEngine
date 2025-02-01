/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-31
 * @license AGPL-3.0
 */

#ifndef MAPCONTROLS_HPP
#define MAPCONTROLS_HPP

#include "core/Controls/CameraControl.hpp"
#include "core/Camera/PerspectiveCamera/PerspectiveCamera.hpp"
#include "core/Renderer/Renderer.hpp"

namespace NoxEngine {

	class MapControls: public CameraControl<PerspectiveCamera> {
		public:
			explicit MapControls(Renderer& renderer, PerspectiveCamera& camera);

			void update() override;

		private:
			Renderer& m_renderer;
			PerspectiveCamera& m_camera;
	};

}

#endif // MAPCONTROLS_HPP
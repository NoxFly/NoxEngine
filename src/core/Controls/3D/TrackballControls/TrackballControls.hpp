/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-31
 * @license AGPL-3.0
 */

#ifndef TRACKBALLCONTROLS_HPP
#define TRACKBALLCONTROLS_HPP

#include "core/Controls/CameraControl.hpp"
#include "core/Camera/3D/PerspectiveCamera/PerspectiveCamera.hpp"
#include "core/Renderer/Renderer.hpp"

namespace NoxEngine {

	class TrackballControls: public CameraControl<V3D, PerspectiveCamera> {
		public:
			explicit TrackballControls(Renderer& renderer, PerspectiveCamera& camera);

			void update() override;

		private:
			Renderer& m_renderer;
			PerspectiveCamera& m_camera;
	};

}

#endif // TRACKBALLCONTROLS_HPP
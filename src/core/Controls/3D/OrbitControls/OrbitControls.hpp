/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-31
 * @license AGPL-3.0
 */

#ifndef ORBITCONTROLS_HPP
#define ORBITCONTROLS_HPP

#include "core/Controls/CameraControl.hpp"
#include "core/Camera/3D/PerspectiveCamera/PerspectiveCamera.hpp"
#include "core/Renderer/Renderer.hpp"

namespace NoxEngine {

	class OrbitControls: public CameraControl<V3D, PerspectiveCamera> {
		public:
			explicit OrbitControls(Renderer& renderer, PerspectiveCamera& camera);
		
			void update() override;

		private:
			Renderer& m_renderer;
			PerspectiveCamera& m_camera;
	};

}
#endif // ORBITCONTROLS_HPP
/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-31
 * @license AGPL-3.0
 */

#ifndef ORBITCONTROLS_HPP
#define ORBITCONTROLS_HPP

#include "NoxEngine/Controls/CameraControl.hpp"
#include "NoxEngine/Camera/PerspectiveCamera.hpp"
#include "NoxEngine/Rendering/Renderer.hpp"

namespace NoxEngine {

	class OrbitControls: public CameraControl<PerspectiveCamera> {
		public:
			explicit OrbitControls(Renderer& renderer, PerspectiveCamera& camera);
		
			void update() override;

		private:
			Renderer& m_renderer;
			PerspectiveCamera& m_camera;
	};

}
#endif // ORBITCONTROLS_HPP
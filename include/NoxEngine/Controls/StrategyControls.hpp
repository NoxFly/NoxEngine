/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-31
 * @license AGPL-3.0
 */

#ifndef STRATEGY_CONTROLS_HPP
#define STRATEGY_CONTROLS_HPP

#include "NoxEngine/Controls/CameraControl.hpp"
#include "NoxEngine/Camera/PerspectiveCamera.hpp"
#include "NoxEngine/Rendering/Renderer.hpp"

namespace NoxEngine {

	class StrategyControls: public CameraControl<PerspectiveCamera> {
		public:
			explicit StrategyControls(Renderer& renderer, PerspectiveCamera& camera);

			void update() override;

		private:
			Renderer& m_renderer;
			PerspectiveCamera& m_camera;
	};

}

#endif // STRATEGY_CONTROLS_HPP
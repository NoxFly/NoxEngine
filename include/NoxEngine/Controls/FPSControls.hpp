/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-31
 * @license AGPL-3.0
 */

#ifndef FPS_CONTROLS_HPP
#define FPS_CONTROLS_HPP

#include "NoxEngine/Controls/PointerLockControls.hpp"

namespace NoxEngine {

	class FPSControls: public PointerLockControls {
		public:
			explicit FPSControls(Renderer& renderer, PerspectiveCamera& camera);
	};

}

#endif // FPS_CONTROLS_HPP
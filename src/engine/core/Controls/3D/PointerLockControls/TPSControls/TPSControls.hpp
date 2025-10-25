/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-31
 * @license AGPL-3.0
 */

#ifndef TPS_CONTROLS_HPP
#define TPS_CONTROLS_HPP

#include "engine/core/Controls/3D/PointerLockControls/PointerLockControls.hpp"

namespace NoxEngine {

	class TPSControls: public PointerLockControls {
		public:
			explicit TPSControls(Renderer& renderer, PerspectiveCamera& camera);
	};

}

#endif // TPS_CONTROLS_HPP
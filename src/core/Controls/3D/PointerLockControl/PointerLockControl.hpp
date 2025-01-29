/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef POINTERLOCKCONTROL_HPP
#define POINTERLOCKCONTROL_HPP

#include "core/Controls/CameraControl.hpp"
#include "core/Camera/3D/PerspectiveCamera/PerspectiveCamera.hpp"
#include "core/Renderer/Input/Input.hpp"

namespace NoxEngine {

	// Specialization for PerspectiveCamera
    template <>
    class CameraControl<PerspectiveCamera> {
        public:
            virtual ~CameraControl() = default;
            virtual void update(PerspectiveCamera& camera, const Input& input, const float deltaTime) = 0;
    };

	class PointerLockControl: public CameraControl<PerspectiveCamera> {
		public:
			explicit PointerLockControl();
			
			void update(PerspectiveCamera& camera, const Input& input, const float deltaTime) override;
	};

}

#endif // POINTERLOCKCONTROL_HPP
/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef POINTERLOCKCONTROL_HPP
#define POINTERLOCKCONTROL_HPP

#include "core/Controls/CameraControl.hpp"
#include "core/Camera/PerspectiveCamera/PerspectiveCamera.hpp"
#include "core/Renderer/Renderer.hpp"

namespace NoxEngine {

	class PointerLockControls: public CameraControl<PerspectiveCamera> {
		public:
			explicit PointerLockControls(Renderer& renderer, PerspectiveCamera& camera);

			void update() override;

			void setSpeed(const float speed) noexcept;
			void setSensitivity(const float sensitivity) noexcept;

			float getSpeed() const noexcept;
			float getSensitivity() const noexcept;

			bool isLocked() noexcept;
			void lockPointer() noexcept;
			void unlockPointer() noexcept;

		private:
			Renderer& m_renderer;
			PerspectiveCamera& m_camera;
			float m_sensitivity, m_speed;
	};

}

#endif // POINTERLOCKCONTROL_HPP
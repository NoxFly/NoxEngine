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
			explicit PointerLockControls(Renderer& renderer, PerspectiveCamera& camera, const bool enableInterpolation = true);

			void update() override;

			void enableInterpolation() noexcept;
			void disableInterpolation() noexcept;
			bool isInterpolationEnabled() const noexcept;

			void setFloatingState(const bool floating) noexcept;
			bool isFloating() const noexcept;

			void setLerpFactor(const float factor) noexcept;

			void setSpeed(const float speed) noexcept;
			void setSensitivity(const float sensitivity) noexcept;

			float getSpeed() const noexcept;
			float getSensitivity() const noexcept;

			bool isLocked() noexcept;
			void lockPointer() noexcept;
			void unlockPointer() noexcept;

		protected:
			Renderer& m_renderer;
			PerspectiveCamera& m_camera;
			float m_sensitivity, m_speed;

			float m_velocity;
			float m_acceleration;
			float m_deceleration;
			float m_lerpFactor;
			V3D m_displacement;
			V2D m_smoothRotation; // Smoothed rotation accumulator for interpolation
			bool m_ignoreNextMouseMove; // Flag to ignore first mouse move after locking pointer
			bool m_enableInterpolation;
			bool m_floating;
	};

}

#endif // POINTERLOCKCONTROL_HPP
/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef POINTERLOCKCONTROL_HPP
#define POINTERLOCKCONTROL_HPP

#include "engine/core/Controls/CameraControl.hpp"
#include "engine/core/Camera/PerspectiveCamera/PerspectiveCamera.hpp"
#include "engine/core/Renderer/Renderer.hpp"

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
			float m_sensitivity, m_speed { 5.0f };

			float m_velocity { 0.0f };
			float m_acceleration { 10.0f };
			float m_deceleration { 5.0f };
			float m_lerpFactor { 10.0f };
			V3D m_displacement { 0.0f, 0.0f, 0.0f };
			V2D m_smoothRotation { 0.0f, 0.0f }; // Smoothed rotation accumulator for interpolation
			bool m_ignoreNextMouseMove { false }; // Flag to ignore first mouse move after locking pointer
			bool m_enableInterpolation { true };
			bool m_floating { false };
	};

}

#endif // POINTERLOCKCONTROL_HPP
/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef CAMERA_2D_HPP
#define CAMERA_2D_HPP

#include <glm/glm.hpp>

#include "core/engine.typedef.hpp"
#include "core/Camera/Camera.hpp"


namespace NoxEngine {

	class Camera2D: public Camera<V2D> {
		public:
			explicit Camera2D();
			~Camera2D();

			V2D getPosition() const noexcept;
			float getZoom() const noexcept;

            void setPosition(const float x, const float y) noexcept;
			void setPosition(const V2D& position) noexcept;

			void zoomIn(const float zoom, const unsigned int duration = 0) noexcept;
			void zoomOut(const float zoom, const unsigned int duration = 0) noexcept;
			void setZoom(const float zoom, const unsigned int duration = 0) noexcept;

			void move(const V2D& offset, unsigned int duration = 0);
			void moveTo(const V2D& position, unsigned int duration = 0);
	};

}

#endif // CAMERA_2D_HPP
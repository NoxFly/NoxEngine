#ifndef CAMERA_2D_HPP
#define CAMERA_2D_HPP

#include <glm/glm.hpp>

#include "engine.typedef.hpp"
#include "Camera.hpp"


namespace NoxEngine {

	class Camera2D: public Camera<V2D> {
		public:
			explicit Camera2D();
			~Camera2D();

			V2D getPosition() const;
			float getZoom() const;

            void setPosition(const float x, const float y);
			void setPosition(const V2D& position);

			void zoomIn(float zoom, uint duration = 0);
			void zoomOut(float zoom, uint duration = 0);
			void setZoom(float zoom, uint duration = 0);

			void move(const V2D& offset, uint duration = 0);
			void moveTo(const V2D& position, uint duration = 0);
	};

}

#endif // CAMERA_2D_HPP
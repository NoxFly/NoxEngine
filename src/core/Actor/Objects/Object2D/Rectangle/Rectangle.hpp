/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include <memory>

#include "core/engine.typedef.hpp"
#include "core/Actor/Objects/Object2D/Object2D.hpp"


namespace NoxEngine {

	class Rectangle: public Object2D {
		public:
			explicit Rectangle();
			explicit Rectangle(V2D& size);
			explicit Rectangle(float width, float height);

			const V2D& size() noexcept;

			~Rectangle();

		protected:
			V2D m_size;
	};

}

#endif // RECTANGLE_HPP
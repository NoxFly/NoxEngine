#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "engine.typedef.hpp"
#include "Object2D.hpp"


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
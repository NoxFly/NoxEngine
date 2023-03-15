#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include "core/Actor/Object2D/Object2D.hpp"


namespace NoxEngine {

	class Circle: public Object2D {
		public:
			explicit Circle();
			explicit Circle(float radius);
			~Circle();

			float radius() const noexcept;

		protected:
			float m_radius;
	};

}

#endif // CIRCLE_HPP
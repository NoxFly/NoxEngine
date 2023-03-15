#ifndef MATRICES_2D_HPP
#define MATRICES_2D_HPP

#include <glm/glm.hpp>

#include "core/engine.typedef.hpp"
#include "core/MatricesMVP/Matrices.hpp"


namespace NoxEngine {

	class Matrices2D: public Matrices<V2D> {
		public:
			Matrices2D();
			~Matrices2D();

			void translate(const V2D& translation) noexcept;
            void rotate(const V2D& rotation) noexcept;
	};

}

#endif // MATRICES_2D_HPP
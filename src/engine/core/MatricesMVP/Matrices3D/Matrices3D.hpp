#ifndef MATRICES_3D_HPP
#define MATRICES_3D_HPP

#include <glm/glm.hpp>

#include "core/engine.typedef.hpp"
#include "core/MatricesMVP/Matrices.hpp"


namespace NoxEngine {

	class Matrices3D: public Matrices<V3D> {
		public:
			explicit Matrices3D(double fov, double aspect, double near, double far, const V3D& position, const V3D& verticalAxis);
			Matrices3D(const Matrices3D& copy);
			~Matrices3D();

			Matrices3D& operator=(const Matrices3D& copy);

			void update() noexcept;

			void translate(const V3D& translation) noexcept;
            void rotate(const V3D& rotation) noexcept;
			
			M4 getProjection() const noexcept;
			M4& getProjection() noexcept;
			

		protected:
			M4 m_projection;
	};

}

#endif // MATRICES_3D_HPP
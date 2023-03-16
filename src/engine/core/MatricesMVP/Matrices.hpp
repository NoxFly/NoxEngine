#ifndef MATRICES_HPP
#define MATRICES_HPP

#include <stack>

#include "core/engine.typedef.hpp"


namespace NoxEngine {

    template <Dimension D>
    class Matrices {

        public:
            explicit Matrices();
            explicit Matrices(M4 view);
            explicit Matrices(M4 view, std::stack<M4> saves);

            explicit Matrices(double fov, double aspect, double near, double far, const V3D& position, const V3D& verticalAxis) requires Is3D<D>;

            virtual ~Matrices() {};

            M4 getModel() const noexcept;
            M4 getView() const noexcept;

            M4& getModel() noexcept;
            M4& getView() noexcept;

            M4 getProjection() const noexcept requires Is3D<D>;
            M4& getProjection() noexcept requires Is3D<D>;

            M4& get() noexcept;

            void setView(const M4& lookAt) noexcept;

            void push() noexcept;
            void pop() noexcept;

            void translate(const V3D& translation) noexcept requires Is3D<D>;
            void rotate(const V3D& rotation) noexcept requires Is3D<D>;

            void translate(const V2D& translation) noexcept requires Is2D<D>;
            void rotate(const V2D& rotation) noexcept requires Is2D<D>;

            

        protected:
            void update() noexcept;

            void _translate(const V3D& translation) noexcept;
            void _rotate(const V3D& rotation) noexcept;

            bool m_needsToUpdate; // in get(), updates is any operation has been done before
            M4 m_model, m_view, m_mvp, m_projection;
            std::stack<M4> m_saves; // stack trace of the pushed mvp versions
    };

}

#include "Matrices.inl"

#endif // MATRICES_HPP
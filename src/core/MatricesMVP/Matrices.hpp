#ifndef MATRICES_HPP
#define MATRICES_HPP

#include <stack>

#include "engine.typedef.hpp"


namespace NoxEngine {

    template <Dimension D>
    class Matrices {

        public:
            explicit Matrices();
            explicit Matrices(M4 view);
            explicit Matrices(M4 view, std::stack<M4> saves);
            virtual ~Matrices() {};

            M4 getModel() const noexcept;
            M4 getView() const noexcept;

            M4& getModel() noexcept;
            M4& getView() noexcept;
            M4& get() noexcept;

            void setView(const M4& lookAt) noexcept;

            void push() noexcept;
            void pop() noexcept;

            virtual void translate(const D& translation) {
                (void) translation;
            };
            
            virtual void rotate(const D& rotation) {
                (void) rotation;
            };


        protected:
            virtual void update() noexcept;

            void _translate(const V3D& translation) noexcept;
            void _rotate(const V3D& rotation) noexcept;

            bool m_needsToUpdate; // in get(), updates is any operation has been done before
            M4 m_model, m_view, m_mvp;
            std::stack<M4> m_saves; // stack trace of the pushed mvp versions
    };

}

#include "Matrices.inl"

#endif // MATRICES_HPP
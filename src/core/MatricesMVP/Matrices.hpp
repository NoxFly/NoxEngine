/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef MATRICES_HPP
#define MATRICES_HPP

#include <stack>

#include "core/engine.typedef.hpp"


namespace NoxEngine {

    class Matrices {

        public:
            explicit Matrices();
            explicit Matrices(const M4& view);
            explicit Matrices(const M4& view, const std::stack<M4>& saves);

            explicit Matrices(const float left, const float right, const float top, const float bottom, const float near, const float far, const V3D& position, const V3D& verticalAxis);
            explicit Matrices(const float fov, const float aspect, const float near, const float far, const V3D& position, const V3D& verticalAxis);

            virtual ~Matrices() {};

            M4 getModel() const noexcept;
            M4 getView() const noexcept;

            M4& getModel() noexcept;
            M4& getView() noexcept;

            M4 getProjection() const noexcept;
            M4& getProjection() noexcept;

            M4& get() noexcept;

            void setView(const M4& lookAt) noexcept;

            void push() noexcept;
            void pop() noexcept;

            void translate(const V3D& translation) noexcept;
            void rotate(const V3D& rotation) noexcept;
            void scale(const V3D& scale) noexcept;

        protected:
            void update() noexcept;

            void _translate(const V3D& translation) noexcept;
            void _rotate(const V3D& rotation) noexcept;
            void _scale(const V3D& scale) noexcept;

            bool m_needsUpdate; // in get(), updates is any operation has been done before
            M4 m_model, m_view, m_mvp, m_projection;
            std::stack<M4> m_saves; // stack trace of the pushed mvp versions
    };

}

#endif // MATRICES_HPP
/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef MOVABLE_OBJECT_HPP
#define MOVABLE_OBJECT_HPP

#include "core/engine.typedef.hpp"

namespace NoxEngine {

    template <Dimension D, bool PublicSetter = true>
    class Movable {

        public:
            explicit Movable(): m_position() {}
            explicit Movable(const D& position): m_position(position) {}

            virtual ~Movable() = 0;

            // GETTERS
            D getPosition() const noexcept {
                return m_position;
            }


            // SETTERS
            void setPosition(const float x, const float y) noexcept requires (Is2D<D> && PublicSetter) {
                m_position = V2D(x, y);
            }

            void setPosition(const float x, const float y, const float z) noexcept requires (Is3D<D> && PublicSetter) {
                m_position = V3D(x, y, z);
            }

            void setPosition(const D& position) noexcept requires PublicSetter {
                m_position = position;
            }


        protected:
            D m_position;
    };

    template <Dimension D, bool P>
    inline Movable<D, P>::~Movable() {}

}

#endif // MOVABLE_OBJECT_HPP
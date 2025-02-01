/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef MOVABLE_OBJECT_HPP
#define MOVABLE_OBJECT_HPP

#include "core/engine.typedef.hpp"

namespace NoxEngine {

    template <bool PublicSetter>
    class Movable {

        public:
            explicit Movable(): m_position() {}
            explicit Movable(const V3D& position): m_position(position) {}

            virtual ~Movable() = 0;

            // GETTERS
            V3D getPosition() const noexcept {
                return m_position;
            }


            // SETTERS
            void setPosition(const float x, const float y, const float z) noexcept requires PublicSetter {
                m_position = V3D(x, y, z);
            }

            void setPosition(const V3D& position) noexcept requires PublicSetter {
                m_position = position;
            }


        protected:
            V3D m_position;
    };

    template <bool P>
    inline Movable<P>::~Movable() {}

}

#endif // MOVABLE_OBJECT_HPP
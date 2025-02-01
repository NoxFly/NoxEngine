/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "core/engine.typedef.hpp"
#include "core/MatricesMVP/Matrices.hpp"
#include "core/Actor/Movable.hpp"
#include "utils/utils.hpp"

namespace NoxEngine {

    class Camera: public Movable<true> {
        public:
            explicit Camera():
                Camera(V3D(0, 0, 0), V3D(0, 0, 0), V3D(0, 1, 0), 0.1f, 1000.f)
            {}

            explicit inline Camera(const V3D& position, const V3D& target, const V3D& verticalAxis, const float near, const float far):
                m_needsUpdate(true),
                m_near(near), m_far(far),
                m_target(target),
                m_verticalAxis(verticalAxis),
                m_matrix()
            {
                _setPosition(position);
            }

            explicit Camera(const float left, const float right, const float top, const float bottom, const float near, const float far):
                m_needsUpdate(true),
                m_near(near), m_far(far),
                m_target(V3D(0, 0, 0)),
                m_verticalAxis(V3D(0, 1, 0)),
                m_matrix(left, right, top, bottom, m_near, m_far, m_position, m_verticalAxis)
            {
                _setPosition(0, 0, 0);
            }

            explicit Camera(const float fov, const float aspect, const float near, const float far) :
                m_near(near), m_far(far),
                m_matrix(fov, aspect, m_near, m_far, m_position, m_verticalAxis)
            {}

            inline virtual ~Camera() {}

            inline virtual void update() noexcept {
                if(m_needsUpdate) {
                    m_needsUpdate = false;
                    m_matrix.setView(glm::lookAt(m_position, m_target, m_verticalAxis));
                }
            }

            inline Matrices& getMatrices() noexcept {
                return m_matrix;
            }

            virtual void move(const V3D& offset, const unsigned int duration = 0) = 0;
            virtual void moveTo(const V3D& position, const unsigned int duration = 0) = 0;

        protected:
            inline void _setPosition(const float x, const float y, const float z) noexcept {
                m_position = V3D(x, y, z);
                m_needsUpdate = true;
            }

            inline void _setPosition(const V3D& position) noexcept {
                m_position = position;
                m_needsUpdate = true;
            }

            bool m_needsUpdate;
            double m_near, m_far;
            V3D m_target, m_verticalAxis;
            Matrices m_matrix;
    };


    

}

#endif // CAMERA_HPP
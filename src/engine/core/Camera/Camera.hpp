#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

#include "core/engine.typedef.hpp"
#include "core/MatricesMVP/Matrices.hpp"
#include "core/Actor/Movable.hpp"
#include "utils/utils.hpp"


namespace NoxEngine {

    template <Dimension D>
    class Camera: public Movable<V3D, false> {
        public:
            explicit Camera():
                Camera(V3D(0, 0, 0), V3D(0, 0, 0), V3D(0, 1, 0))
            {}

            explicit Camera(const V3D& position, const V3D& target, const V3D& verticalAxis):
                m_needsUpdate(true),
                m_target(target),
                m_verticalAxis(verticalAxis),
                m_matrix()
            {
                _setPosition(position);
            };

            virtual ~Camera() {};
            
            virtual void update() {
                if(m_needsUpdate) {
                    m_needsUpdate = false;
                    m_matrix.setView(glm::lookAt(m_position, m_target, m_verticalAxis));
                }
            };

            virtual void setPosition(const D& position) = 0;
            
            virtual D getPosition() const = 0;

            virtual void move(const D& offset, unsigned int duration = 0) = 0;
            virtual void moveTo(const D& position, unsigned int duration = 0) = 0;

            Matrices<D>& getMatrices() {
                return m_matrix;
            }

        protected:
            void _setPosition(const float x, const float y, const float z) {
                m_position = V3D(x, y, z);
                m_needsUpdate = true;
            }

            void _setPosition(const V3D& position) {
                m_position = position;
                m_needsUpdate = true;
            }

            bool m_needsUpdate;
            V3D m_target, m_verticalAxis;
            Matrices<D> m_matrix;
    };

}

#endif // CAMERA_HPP
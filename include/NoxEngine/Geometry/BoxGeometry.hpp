/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef BOX_GEOMETRY_HPP
#define BOX_GEOMETRY_HPP

#include <glm/glm.hpp>

#include "NoxEngine/engine.typedef.hpp"
#include "NoxEngine/Geometry/BufferGeometry.hpp"


namespace NoxEngine {

    class BoxGeometry: public BufferGeometry {
        public:
            static std::shared_ptr<BoxGeometry> create(float width = 1.f, float height = 1.f, float depth = 1.f);
            static std::shared_ptr<BoxGeometry> create(const V3D& size);

            explicit BoxGeometry(const float width, const float height, const float depth);
            explicit BoxGeometry(const V3D& size);
            ~BoxGeometry() = default;
            
            BoxGeometry(const BoxGeometry&) = delete;
            BoxGeometry& operator=(const BoxGeometry&) = delete;

        private:
            void buildGeometry(float width, float height, float depth);
    };

}

#endif // BOX_GEOMETRY_HPP
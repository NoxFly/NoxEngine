/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef CUBE_HPP
#define CUBE_HPP

#include <string>
#include <glm/glm.hpp>
#include <memory>

#include "engine/Rendering/Components/Mesh3D/Mesh3D.hpp"
#include "utils/Color.hpp"


namespace NoxEngine {

    class Cube: public Mesh3D {
        public:
            explicit Cube(const float size);
            explicit Cube(const float size, const Color& color);
            explicit Cube(const float size, const std::string& textureName);
            explicit Cube(const float size, const std::string& textureName, const Color& color);
            ~Cube() = default;
    };

}

#endif // CUBE_HPP
/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef OBJ_LOADER_HPP
#define OBJ_LOADER_HPP

#include <string>
#include <memory>

#include "engine/Rendering/Materials/Material.hpp"
#include "engine/Rendering/Components/Mesh/MeshData.hpp"
#include "engine/Rendering/Geometries/BufferGeometry.hpp"
#include "utils/Color.hpp"

namespace NoxEngine {

    class OBJLoader {
        public:
            static MeshData load(const std::string& filepath);
    };

}

#endif // OBJ_LOADER_HPP
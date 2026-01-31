/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef OBJ_LOADER_HPP
#define OBJ_LOADER_HPP

#include <string>
#include <memory>

#include "NoxEngine/Material/Material.hpp"
#include "NoxEngine/Mesh/MeshData.hpp"
#include "NoxEngine/Geometry/BufferGeometry.hpp"
#include "NoxEngine/utils/Color.hpp"

namespace NoxEngine {

    class OBJLoader {
        public:
            static MeshData load(const std::string& filepath);
    };

}

#endif // OBJ_LOADER_HPP
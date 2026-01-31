/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef MESH_DATA_HPP
#define MESH_DATA_HPP

#include <memory>

#include "NoxEngine/Geometry/BufferGeometry.hpp"
#include "NoxEngine/Material/Material.hpp"

namespace NoxEngine {
    // class BufferGeometry;
    // class Material;

    struct MeshData {
        std::shared_ptr<BufferGeometry> geometry;
        std::shared_ptr<Material> material; // nullptr si absent
    };
}

#endif // MESH_DATA_HPP
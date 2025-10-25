/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef MESH_DATA_HPP
#define MESH_DATA_HPP

#include <memory>

#include "engine/Rendering/Geometries/BufferGeometry.hpp"
#include "engine/Rendering/Materials/Material.hpp"

namespace NoxEngine {
    // class BufferGeometry;
    // class Material;

    struct MeshData {
        std::shared_ptr<BufferGeometry> geometry;
        std::shared_ptr<Material> material; // nullptr si absent
    };
}

#endif // MESH_DATA_HPP
/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "BoxGeometry.hpp"


namespace NoxEngine {

    std::shared_ptr<BoxGeometry> BoxGeometry::create(float width, float height, float depth) {
        return std::make_shared<BoxGeometry>(width, height, depth);
    }

    std::shared_ptr<BoxGeometry> BoxGeometry::create(const V3D& size) {
        return std::make_shared<BoxGeometry>(size);
    }

    BoxGeometry::BoxGeometry(const float width, const float height, const float depth) {
        buildGeometry(width, height, depth);
    }

    BoxGeometry::BoxGeometry(const V3D& size) {
        buildGeometry(size.x, size.y, size.z);
    }

    void BoxGeometry::buildGeometry(const float width, const float height, const float depth) {
        // --- positions et normales d'un cube ---
        float hw = width * 0.5f;
        float hh = height * 0.5f;
        float hd = depth * 0.5f;

        std::vector<float> positions = {
            // Front
            -hw,-hh, hd,  hw,-hh, hd,  hw,hh, hd, -hw,hh, hd,
            // Back
            hw,-hh,-hd, -hw,-hh,-hd, -hw,hh,-hd, hw,hh,-hd,
            // Left
            -hw,-hh,-hd, -hw,-hh, hd, -hw,hh, hd, -hw,hh,-hd,
            // Right
            hw,-hh, hd, hw,-hh,-hd, hw,hh,-hd, hw,hh, hd,
            // Top
            -hw, hh, hd,  hw, hh, hd,  hw,hh,-hd, -hw,hh,-hd,
            // Bottom
            -hw,-hh,-hd,  hw,-hh,-hd,  hw,-hh, hd, -hw,-hh, hd
        };

        std::vector<float> normals = {
            // Front
            0,0,1, 0,0,1, 0,0,1, 0,0,1,
            // Back
            0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1,
            // Left
            -1,0,0, -1,0,0, -1,0,0, -1,0,0,
            // Right
            1,0,0, 1,0,0, 1,0,0, 1,0,0,
            // Top
            0,1,0, 0,1,0, 0,1,0, 0,1,0,
            // Bottom
            0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0
        };

        std::vector<float> uvs = {
            0,0, 1,0, 1,1, 0,1,   // Front
            0,0, 1,0, 1,1, 0,1,   // Back
            0,0, 1,0, 1,1, 0,1,   // Left
            0,0, 1,0, 1,1, 0,1,   // Right
            0,0, 1,0, 1,1, 0,1,   // Top
            0,0, 1,0, 1,1, 0,1    // Bottom
        };

        std::vector<unsigned int> indices = {
            0,1,2, 2,3,0,       // Front
            4,5,6, 6,7,4,       // Back
            8,9,10, 10,11,8,    // Left
            12,13,14, 14,15,12, // Right
            16,17,18, 18,19,16, // Top
            20,21,22, 22,23,20  // Bottom
        };

        setPositions(positions);
        setNormals(normals);
        setUVs(uvs);
        setIndices(indices);
    }

}
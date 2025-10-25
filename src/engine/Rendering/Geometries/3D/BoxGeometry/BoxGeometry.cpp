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
        // 8 sommets d’un cube
        std::vector<float> vertices = {
            -width/2, -height/2, -depth/2,
            width/2, -height/2, -depth/2,
            width/2,  height/2, -depth/2,
            -width/2,  height/2, -depth/2,
            -width/2, -height/2,  depth/2,
            width/2, -height/2,  depth/2,
            width/2,  height/2,  depth/2,
            -width/2,  height/2,  depth/2,
        };

        // Normales (6 faces)
        std::vector<float> normals = {
            // -Z
            0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f,
            // +Z
            0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f,
            // -X
            -1.f,0.f,0.f,-1.f,0.f,0.f,-1.f,0.f,0.f,-1.f,0.f,0.f,
            // +X
            1.f,0.f,0.f,1.f,0.f,0.f,1.f,0.f,0.f,1.f,0.f,0.f,
            // -Y
            0.f,-1.f,0.f,0.f,-1.f,0.f,0.f,-1.f,0.f,0.f,-1.f,0.f,
            // +Y
            0.f,1.f,0.f,0.f,1.f,0.f,0.f,1.f,0.f,0.f,1.f,0.f
        };

        // UV basiques pour chaque face
        std::vector<float> uvs = {
            0.f,0.f, 1.f,0.f, 1.f,1.f, 0.f,1.f,
            0.f,0.f, 1.f,0.f, 1.f,1.f, 0.f,1.f,
            0.f,0.f, 1.f,0.f, 1.f,1.f, 0.f,1.f,
            0.f,0.f, 1.f,0.f, 1.f,1.f, 0.f,1.f,
            0.f,0.f, 1.f,0.f, 1.f,1.f, 0.f,1.f,
            0.f,0.f, 1.f,0.f, 1.f,1.f, 0.f,1.f
        };

        // Indices pour 12 triangles (2 par face)
        std::vector<uint> indices = {
            0,1,2, 2,3,0,   // -Z
            4,5,6, 6,7,4,   // +Z
            0,3,7, 7,4,0,   // -X
            1,5,6, 6,2,1,   // +X
            0,1,5, 5,4,0,   // -Y
            3,2,6, 6,7,3    // +Y
        };

        setPositions(vertices);
        setNormals(normals);
        setUVs(uvs);
        setIndices(indices);
    }

}
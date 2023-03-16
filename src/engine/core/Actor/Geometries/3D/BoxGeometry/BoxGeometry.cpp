#include "BoxGeometry.hpp"


namespace NoxEngine {

    BoxGeometry::BoxGeometry(const float width, const float height, const float depth):
        BoxGeometry(V3D(width, height, depth))
    {

    }

    BoxGeometry::BoxGeometry(const V3D& size):
        BufferGeometry()
    {
        const float sw = size.x / 2.f;
        const float sh = size.y / 2.f;
        const float sd = size.z / 2.f;

        m_data.vertices = {
            // front
            -sw, -sh,  sd,
             sw, -sh,  sd,
             sw,  sh,  sd,
            -sw,  sh,  sd,
            // back
            -sw, -sh, -sd,
             sw, -sh, -sd,
             sw,  sh, -sd,
            -sw,  sh, -sd
        };

        /*m_data.uvs = {
            0, 0,   0, 1,   1, 1,
            0, 0,   1, 0,   1, 1,

            0, 0,   0, 1,   1, 1,
            0, 0,   1, 0,   1, 1,

            0, 0,   0, 1,   1, 1,
            0, 0,   1, 0,   1, 1,

            0, 0,   0, 1,   1, 1,
            0, 0,   1, 0,   1, 1,

            0, 0,   0, 1,   1, 1,
            0, 0,   1, 0,   1, 1,

            0, 0,   0, 1,   1, 1,
            0, 0,   1, 0,   1, 1
        };*/

        m_data.colors = {
            1.f, 0.f, 0.f
        };

        m_data.indices = {
            // front
            0, 1, 2,
            2, 3, 0,
            // right
            1, 5, 6,
            6, 2, 1,
            // back
            7, 6, 5,
            5, 4, 7,
            // left
            4, 0, 3,
            3, 7, 4,
            // bottom
            4, 5, 1,
            1, 0, 4,
            // top
            3, 2, 6,
            6, 7, 3
        };

        load();

    }

    BoxGeometry::~BoxGeometry() {

    }

}
#include "BoxGeometry.hpp"


namespace NoxEngine {

    BoxGeometry::BoxGeometry(const float width, const float height, const float depth):
        BoxGeometry(V3D(width, height, depth))
    {

    }

    BoxGeometry::BoxGeometry(const V3D& size):
        Geometry()
    {
        const float sw = size.x / 2;
        const float sh = size.y / 2;
        const float sd = size.z / 2;

        const std::vector<float> vertices = {
            -sw, -sh, -sd,      sw, -sh, -sd,      sw,  sh, -sd,
            -sw, -sh, -sd,     -sw,  sh, -sd,      sw,  sh, -sd,

            sw, -sh,  sd,      sw, -sh, -sd,      sw,  sh, -sd,
            sw, -sh,  sd,      sw,  sh,  sd,      sw,  sh, -sd,

            -sw, -sh,  sd,      sw, -sh,  sd,      sw, -sh, -sd,
            -sw, -sh,  sd,     -sw, -sh, -sd,      sw, -sh, -sd,

            -sw, -sh,  sd,      sw, -sh,  sd,      sw,  sh,  sd,
            -sw, -sh,  sd,     -sw,  sh,  sd,      sw,  sh,  sd,

            -sw, -sh, -sd,     -sw, -sh,  sd,     -sw,  sh,  sd,
            -sw, -sh, -sd,     -sw,  sh, -sd,     -sw,  sh,  sd,

            -sw,  sh,  sd,      sw,  sh,  sd,      sw,  sh, -sd,
            -sw,  sh,  sd,     -sw,  sh, -sd,      sw,  sh, -sd
        };

        const std::vector<float> textures = {
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
        };

        setVertices(vertices);
        setTextures(textures);
    }

    BoxGeometry::~BoxGeometry() {

    }

}
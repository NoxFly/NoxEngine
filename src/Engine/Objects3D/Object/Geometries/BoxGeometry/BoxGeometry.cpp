#include "BoxGeometry.hpp"

BoxGeometry::BoxGeometry(const float width, const float height, const float depth):
    BoxGeometry(glm::vec3(width, height, depth))
{

}

BoxGeometry::BoxGeometry(const glm::vec3& size):
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

    setVertices(vertices);
}

BoxGeometry::~BoxGeometry() {

}

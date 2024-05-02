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


        GeometryData data;
        GeometryIndexedData<V3D> indexedData;

        indexedData.vertices = {
            // front
            { -sw, -sh,  sd },
            {  sw, -sh,  sd },
            {  sw,  sh,  sd },
            { -sw,  sh,  sd },
            // back
            { -sw, -sh, -sd },
            {  sw, -sh, -sd },
            {  sw,  sh, -sd },
            { -sw,  sh, -sd }
        };

        indexedData.uvs = {
            // Front face
            { 0, 0 },
            { 1, 0 },
            { 1, 1 },
            { 0, 1 },
        };

        indexedData.normals = {
            {  0.0f,  1.0f,  0.0f },
            {  0.0f,  0.0f,  1.0f },
            { -1.0f,  0.0f,  0.0f },
            {  0.0f, -1.0f,  0.0f },
            {  1.0f,  0.0f,  0.0f },
            {  0.0f,  0.0f, -1.0f }
        };

        // vertices, uvs, normals, colors
        indexedData.indexes = {
            // front
            { 1, 1, 1, 0 }, { 2, 2, 1, 0 }, { 3, 3, 1, 0 },
            { 3, 3, 1, 0 }, { 4, 4, 1, 0 }, { 1, 1, 1, 0 },
            // right
            { 2, 1, 2, 0 }, { 6, 2, 2, 0 }, { 7, 3, 2, 0 },
            { 7, 3, 2, 0 }, { 3, 4, 2, 0 }, { 2, 1, 2, 0 },
            // back
            { 8, 1, 3, 0 }, { 7, 2, 3, 0 }, { 6, 3, 3, 0 },
            { 6, 3, 3, 0 }, { 5, 4, 3, 0 }, { 8, 1, 3, 0 },
            // left
            { 5, 1, 4, 0 }, { 1, 2, 4, 0 }, { 4, 3, 4, 0 },
            { 4, 3, 4, 0 }, { 8, 4, 4, 0 }, { 5, 1, 4, 0 },
            // bottom
            { 5, 1, 5, 0 }, { 6, 2, 5, 0 }, { 2, 3, 5, 0 },
            { 2, 3, 5, 0 }, { 1, 4, 5, 0 }, { 5, 1, 5, 0 },
            // top
            { 4, 1, 6, 0 }, { 3, 2, 6, 0 }, { 7, 3, 6, 0 },
            { 7, 3, 6, 0 }, { 8, 4, 6, 0 }, { 4, 1, 6, 0 }
        };


        indexData(indexedData, data);

        load(data);
    }

    BoxGeometry::~BoxGeometry() {

    }

}
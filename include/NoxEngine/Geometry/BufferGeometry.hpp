/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef BUFFER_GEOMETRY_HPP
#define BUFFER_GEOMETRY_HPP

#include <memory>
#include <vector>
#include <GL/glew.h>

#include "NoxEngine/engine.typedef.hpp"

namespace NoxEngine {
    
    class BufferGeometry {
        public:
            static std::shared_ptr<BufferGeometry> create();
            
            ~BufferGeometry();

            BufferGeometry(const BufferGeometry&) = delete;
            BufferGeometry& operator=(const BufferGeometry&) = delete;

            BufferGeometry(BufferGeometry&&) noexcept;
            BufferGeometry& operator=(BufferGeometry&&) noexcept;


            // setters (style C++ strict)
            void setPositions(const std::vector<float>& positions); // x,y,z...
            void setNormals(const std::vector<float>& normals);     // x,y,z...
            void setUVs(const std::vector<float>& uvs);             // u,v...
            void setColors(const std::vector<float>& colors);       // r,g,b...
            void setIndices(const std::vector<uint>& indices);

            // query
            bool hasPositions() const noexcept;
            bool hasNormals() const noexcept;
            bool hasUVs() const noexcept;
            bool hasColors() const noexcept;
            bool hasIndices() const noexcept;

            // Called by Mesh before draw. Will upload to GPU if not already.
            void bind() const noexcept;

            // draw call helper (expects material/shader to be active)
            void draw() const noexcept;

            // getters
            GLuint getVAO() const noexcept { return m_vao; }
            GLuint getElementCount() const noexcept { return m_elementCount; }
            
        protected:
            explicit BufferGeometry() = default;

            // Upload performed lazily on first bind/draw
            void uploadToGPU() const noexcept;

            void deleteGPUResources() noexcept;

            // mutable because upload happens in const bind()
            mutable bool m_uploaded { false };

            // CPU-side data
            std::vector<float> m_positions;
            std::vector<float> m_normals;
            std::vector<float> m_uvs;
            std::vector<float> m_colors;
            std::vector<uint> m_indices;

            // GPU handles
            mutable GLuint m_vao { 0 };
            mutable GLuint m_vboPositions { 0 };
            mutable GLuint m_vboNormals { 0 };
            mutable GLuint m_vboUVs { 0 };
            mutable GLuint m_vboColors { 0 };
            mutable GLuint m_ebo { 0 };

            mutable GLuint m_vertexCount { 0 };   // number of vertices (positions.size()/3)
            mutable GLuint m_elementCount { 0 };  // number of indices
    };

    typedef BufferGeometry Geometry;

}

#endif // BUFFER_GEOMETRY_HPP
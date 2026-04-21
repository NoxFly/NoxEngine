// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

#include <cstdint>
#include <vector>

namespace Nox {

    /// GPU-based occlusion culling using Hierarchical Z-Buffer (HZB).
    /// Generates a depth mip chain from the depth buffer, then tests
    /// object bounding boxes against the HZB to determine visibility.
    class OcclusionCulling {
    public:
        OcclusionCulling() = default;
        ~OcclusionCulling();

        OcclusionCulling(const OcclusionCulling&) = delete;
        OcclusionCulling& operator=(const OcclusionCulling&) = delete;
        OcclusionCulling(OcclusionCulling&&) = default;
        OcclusionCulling& operator=(OcclusionCulling&&) = default;

        /// Initialize the HZB resources.
        void init(int depthWidth, int depthHeight);

        /// Resize the HZB when the viewport changes.
        void resize(int depthWidth, int depthHeight);

        /// Generate the HZB mip chain from the scene depth buffer.
        void buildHZB(uint32_t depthTexture);

        /// Test an AABB against the HZB. Returns true if potentially visible.
        [[nodiscard]] bool testAABB(const Math::AABB& worldAABB,
                                     const Math::Mat4& viewProjMatrix) const;

        /// Batch test multiple AABBs and return visibility results.
        /// Uses a compute shader for GPU-accelerated testing.
        void testAABBs(const std::vector<Math::AABB>& aabbs,
                       const Math::Mat4& viewProjMatrix,
                       std::vector<bool>& outVisibility);

        void setEnabled(bool enabled) { enabled_ = enabled; }
        [[nodiscard]] bool isEnabled() const { return enabled_; }
        [[nodiscard]] bool isInitialized() const { return initialized_; }
        [[nodiscard]] int hzbLevels() const { return numLevels_; }

    private:
        void buildMipChain();
        void destroy();

        uint32_t hzbTexture_     = 0;    ///< HZB mip chain texture
        uint32_t hzbFBO_         = 0;    ///< FBO for mip generation
        uint32_t downsampleProgram_ = 0; ///< Compute shader for mip downsampling
        uint32_t testProgram_    = 0;    ///< Compute shader for AABB testing
        uint32_t resultSSBO_     = 0;    ///< Visibility results buffer

        int width_       = 0;
        int height_      = 0;
        int numLevels_   = 0;
        bool enabled_    = true;
        bool initialized_ = false;
    };

} // namespace Nox

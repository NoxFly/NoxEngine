// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

#include <cstdint>
#include <span>
#include <vector>

namespace Nox {

    /// Represents a single indirect draw command compatible with
    /// glDrawElementsIndirect / GL_DRAW_INDIRECT_BUFFER.
    struct DrawElementsIndirectCommand {
        uint32_t count         = 0; ///< Index count per instance
        uint32_t instanceCount = 1; ///< Number of instances
        uint32_t firstIndex    = 0; ///< Offset in index buffer
        int32_t  baseVertex    = 0; ///< Base vertex offset
        uint32_t baseInstance  = 0; ///< Base instance for instanced attributes
    };

    /// Manages a GPU-side indirect draw buffer.
    /// All draw calls are recorded CPU-side and then flushed to a single
    /// GL_DRAW_INDIRECT_BUFFER for multi-draw-indirect execution.
    class IndirectDrawBuffer {
    public:
        IndirectDrawBuffer() = default;
        ~IndirectDrawBuffer();

        IndirectDrawBuffer(const IndirectDrawBuffer&) = delete;
        IndirectDrawBuffer& operator=(const IndirectDrawBuffer&) = delete;
        IndirectDrawBuffer(IndirectDrawBuffer&&) noexcept;
        IndirectDrawBuffer& operator=(IndirectDrawBuffer&&) noexcept;

        /// Record a draw command.
        void addCommand(const DrawElementsIndirectCommand& cmd);

        /// Clear all recorded commands.
        void clear();

        /// Upload commands to GPU and execute multi-draw-indirect.
        /// VAO must be bound before calling this.
        void flush();

        [[nodiscard]] int commandCount() const { return static_cast<int>(commands_.size()); }
        [[nodiscard]] bool empty() const { return commands_.empty(); }

    private:
        void ensureBuffer();

        std::vector<DrawElementsIndirectCommand> commands_;
        uint32_t bufferObject_ = 0;
        uint32_t bufferCapacity_ = 0; ///< Current GPU buffer capacity in commands
    };

} // namespace Nox

// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/IndirectDraw.hpp>

#include <GL/glew.h>

#include <algorithm>
#include <utility>

namespace Nox {

    IndirectDrawBuffer::~IndirectDrawBuffer() {
        if (bufferObject_ != 0) {
            glDeleteBuffers(1, &bufferObject_);
        }
    }

    IndirectDrawBuffer::IndirectDrawBuffer(IndirectDrawBuffer&& other) noexcept
        : commands_(std::move(other.commands_))
        , bufferObject_(std::exchange(other.bufferObject_, 0))
        , bufferCapacity_(std::exchange(other.bufferCapacity_, 0))
    {}

    IndirectDrawBuffer& IndirectDrawBuffer::operator=(IndirectDrawBuffer&& other) noexcept {
        if (this != &other) {
            if (bufferObject_ != 0) {
                glDeleteBuffers(1, &bufferObject_);
            }
            commands_ = std::move(other.commands_);
            bufferObject_ = std::exchange(other.bufferObject_, 0);
            bufferCapacity_ = std::exchange(other.bufferCapacity_, 0);
        }
        return *this;
    }

    void IndirectDrawBuffer::addCommand(const DrawElementsIndirectCommand& cmd) {
        commands_.push_back(cmd);
    }

    void IndirectDrawBuffer::clear() {
        commands_.clear();
    }

    void IndirectDrawBuffer::ensureBuffer() {
        if (bufferObject_ == 0) {
            glCreateBuffers(1, &bufferObject_);
        }
    }

    void IndirectDrawBuffer::flush() {
        if (commands_.empty()) {
            return;
        }

        ensureBuffer();

        auto neededCapacity = static_cast<uint32_t>(commands_.size());
        auto byteSize = static_cast<GLsizeiptr>(neededCapacity * sizeof(DrawElementsIndirectCommand));

        if (neededCapacity > bufferCapacity_) {
            // Reallocate with some headroom
            bufferCapacity_ = std::max(neededCapacity, bufferCapacity_ * 2);
            auto allocSize = static_cast<GLsizeiptr>(bufferCapacity_ * sizeof(DrawElementsIndirectCommand));
            glNamedBufferData(bufferObject_, allocSize, nullptr, GL_DYNAMIC_DRAW);
        }

        // Upload commands
        glNamedBufferSubData(bufferObject_, 0, byteSize, commands_.data());

        // Execute multi-draw-indirect
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, bufferObject_);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr,
                                     static_cast<GLsizei>(commands_.size()),
                                     sizeof(DrawElementsIndirectCommand));
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    }

} // namespace Nox

// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/core/Serialization.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <cstring>
#include <fstream>

namespace Nox {

    void Archive::write(const void* data, size_t size) {
        const auto* bytes = static_cast<const uint8_t*>(data);
        data_.insert(data_.end(), bytes, bytes + size);
    }

    bool Archive::read(void* data, size_t size) {
        if (readPos_ + size > data_.size()) { return false; }
        std::memcpy(data, data_.data() + readPos_, size);
        readPos_ += size;
        return true;
    }

    void Archive::writeString(std::string_view str) {
        auto len = static_cast<uint32_t>(str.size());
        write(len);
        write(str.data(), str.size());
    }

    std::string Archive::readString() {
        uint32_t len = 0;
        if (!read(len)) { return {}; }
        std::string result(len, '\0');
        if (!read(result.data(), len)) { return {}; }
        return result;
    }

    bool Archive::saveToFile(const std::filesystem::path& path) const {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            NOX_LOG_ERROR("Failed to save archive to: {}", path.string());
            return false;
        }
        file.write(reinterpret_cast<const char*>(data_.data()),
                    static_cast<std::streamsize>(data_.size()));
        return file.good();
    }

    bool Archive::loadFromFile(const std::filesystem::path& path) {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            NOX_LOG_ERROR("Failed to load archive from: {}", path.string());
            return false;
        }
        auto fileSize = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);
        data_.resize(fileSize);
        file.read(reinterpret_cast<char*>(data_.data()),
                   static_cast<std::streamsize>(fileSize));
        readPos_ = 0;
        return file.good();
    }

    void Archive::resetRead() {
        readPos_ = 0;
    }

    void Archive::clear() {
        data_.clear();
        readPos_ = 0;
    }

} // namespace Nox

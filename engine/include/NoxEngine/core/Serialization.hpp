// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Nox {

    /// Simple serialization archive supporting binary read/write.
    class Archive {
    public:
        Archive() = default;

        /// Write raw bytes to the archive.
        void write(const void* data, size_t size);

        /// Read raw bytes from the archive.
        bool read(void* data, size_t size);

        /// Write a trivially-copyable value.
        template <typename T>
            requires std::is_trivially_copyable_v<T>
        void write(const T& value) {
            write(&value, sizeof(T));
        }

        /// Read a trivially-copyable value.
        template <typename T>
            requires std::is_trivially_copyable_v<T>
        bool read(T& value) {
            return read(&value, sizeof(T));
        }

        /// Write a string (length-prefixed).
        void writeString(std::string_view str);

        /// Read a string (length-prefixed).
        [[nodiscard]] std::string readString();

        /// Save archive to file.
        [[nodiscard]] bool saveToFile(const std::filesystem::path& path) const;

        /// Load archive from file.
        [[nodiscard]] bool loadFromFile(const std::filesystem::path& path);

        /// Reset read position to the beginning.
        void resetRead();

        /// Clear all data.
        void clear();

        [[nodiscard]] size_t size() const { return data_.size(); }
        [[nodiscard]] const std::vector<uint8_t>& data() const { return data_; }

    private:
        std::vector<uint8_t> data_;
        size_t readPos_ = 0;
    };

    /// Interface for objects that can be serialized.
    class ISerializable {
    public:
        virtual ~ISerializable() = default;
        virtual void serialize(Archive& archive) const = 0;
        virtual void deserialize(Archive& archive) = 0;
    };

} // namespace Nox

// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <cstdint>
#include <filesystem>
#include <string_view>

namespace Nox {

    struct TextureData {
        int      width    = 0;
        int      height   = 0;
        int      channels = 0;
        uint8_t* pixels   = nullptr;
    };

    class TextureLoader {
    public:
        [[nodiscard]] static TextureData load(const std::filesystem::path& path);
        static void free(TextureData& data);
    };

} // namespace Nox

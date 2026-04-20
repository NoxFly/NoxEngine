// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/TextureLoader.hpp>
#include <NoxEngine/core/Logger.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Nox {

    TextureData TextureLoader::load(const std::filesystem::path& path) {
        TextureData data;

        stbi_set_flip_vertically_on_load(1);
        data.pixels = stbi_load(path.string().c_str(), &data.width, &data.height, &data.channels, 4);

        if (!data.pixels) {
            NOX_LOG_ERROR("Failed to load texture: {}", path.string());
            return data;
        }

        data.channels = 4; // We force RGBA
        NOX_LOG_INFO("Loaded texture: {} ({}x{}, {} channels)", path.string(), data.width, data.height, data.channels);
        return data;
    }

    void TextureLoader::free(TextureData& data) {
        if (data.pixels) {
            stbi_image_free(data.pixels);
            data.pixels = nullptr;
        }
    }

} // namespace Nox

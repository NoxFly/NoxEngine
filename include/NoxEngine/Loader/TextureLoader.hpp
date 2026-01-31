/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef TEXTURE_LOADER_HPP
#define TEXTURE_LOADER_HPP

#include <string>
#include <memory>
#include <unordered_map>

#include "NoxEngine/Rendering/Texture.hpp"

namespace NoxEngine {

    class TextureLoader {
        public:
            static std::shared_ptr<Texture> load(const std::string& textureName, const std::string& filepath);
            static std::shared_ptr<Texture> get(const std::string& textureName);

        private:
            inline static std::unordered_map<std::string, std::shared_ptr<Texture>> s_loadedTextures;

            static SDL_Surface* invertPixels(SDL_Surface *src);
    };

}

#endif // TEXTURE_LOADER_HPP
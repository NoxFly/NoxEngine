/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include <fstream>
#include <sstream>
#include <filesystem>

#include "NoxEngine/Loader/TextureLoader.hpp"
#include "internal/Console.hpp"

namespace fs = std::filesystem;

namespace NoxEngine {

	std::shared_ptr<Texture> TextureLoader::get(const std::string& textureName) {
        if(s_loadedTextures.find(textureName) != s_loadedTextures.end()) {
            return s_loadedTextures[textureName];
        }

        throw std::runtime_error("Texture " + textureName + " not loaded");
    }

    std::shared_ptr<Texture> TextureLoader::load(const std::string& textureName, const std::string& filepath) {
		if(s_loadedTextures.find(textureName) != s_loadedTextures.end()) {
			return s_loadedTextures[textureName];
		}

        SDL_Surface* image = IMG_Load(filepath.c_str());

        if(image == 0) {
            std::string sdlErrorMsg = SDL_GetError();
            Console::error("Texture::load", "Failed to load the texture : " + sdlErrorMsg);
            throw std::runtime_error("Failed to load the texture : " + sdlErrorMsg);
        }

        SDL_Surface* invertedImage = invertPixels(image);
        SDL_FreeSurface(image);

		GLuint textureId = 0;

        glGenTextures(1, &textureId);

        glBindTexture(GL_TEXTURE_2D, textureId);

        GLenum internalFormat = 0;
        GLenum format = 0;

        if(invertedImage->format->BytesPerPixel == 3) {
            internalFormat = GL_RGB;
            format = (invertedImage->format->Rmask == 0xff)? GL_RGB : GL_BGR;
        }
        else if(invertedImage->format->BytesPerPixel == 4) {
            internalFormat = GL_RGBA;
            format = (invertedImage->format->Rmask == 0xff)? GL_RGBA : GL_BGRA;
        }
        else {
            Console::error("Texture::load", "Unknown internal image format");
            SDL_FreeSurface(invertedImage);
            throw std::runtime_error("Unknown internal image format");
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, invertedImage->w, invertedImage->h, 0, format, GL_UNSIGNED_BYTE, invertedImage->pixels);

        // filters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // unlock
        glBindTexture(GL_TEXTURE_2D, 0);

        SDL_FreeSurface(invertedImage);

		auto texture = std::make_shared<Texture>(filepath, textureName);
		texture->m_id = textureId;

		s_loadedTextures[textureName] = texture;

		return texture;
    }

    SDL_Surface* TextureLoader::invertPixels(SDL_Surface *src) {
        // copy source image in pixels
        SDL_Surface *invertedImage = SDL_CreateRGBSurface(
            0,
            src->w, src->h,
            src->format->BitsPerPixel,
            src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask
        );

        // tmp array to manipulate pixels
        unsigned char* srcPixels = (unsigned char*) src->pixels;
        unsigned char* invertedPixels = (unsigned char*) invertedImage->pixels;

        // pixels reverse
        for(int i = 0; i < src->h; i++) {
            for(int j = 0; j < src->w * src->format->BytesPerPixel; j++)
                invertedPixels[(src->w * src->format->BytesPerPixel * (src->h - 1 - i)) + j] = srcPixels[(src->w * src->format->BytesPerPixel * i) + j];
        }

        return invertedImage;
    }

}
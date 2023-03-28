#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>

#include "ResourceHolder/ResourceHolder.hpp"


namespace NoxEngine {

    class Texture {
        public:
            static void setTexturesPath(const std::string& texturesPath) noexcept;
            static bool load(const std::string& texturePath, const std::string& textureName);
            static Texture* get(const std::string& textureName) noexcept;

            explicit Texture(const std::string& texturePath, const std::string& textureName);
            explicit Texture(Texture const& copy);
            
            ~Texture();

            Texture& operator=(Texture const &copy);

            bool load();

            GLuint getID() const noexcept;
            std::string getName() const noexcept;
            std::string getPath() const noexcept;

        private:
            static std::string texturesPath;
            static ResourceHolder<Texture, std::string> m_bank;

            SDL_Surface* invertPixels(SDL_Surface *src) const;

            GLuint m_id;
            std::string m_name;
            std::string m_path;
    };

}

#endif // TEXTURE_HPP
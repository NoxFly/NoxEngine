#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>

#include "ResourceHolder.hpp"

class Texture {
	public:
        static void setTexturesPath(const std::string& texturesPath);
        static bool load(const std::string& texturePath, const std::string& textureName);
        static Texture* get(const std::string& textureName);

		Texture(const std::string& texturePath, const std::string& textureName);
        Texture(Texture const& copy);
		~Texture();

        Texture& operator=(Texture const &copy);

        bool load();

        GLuint getID() const;
        std::string getName() const;
        std::string getPath() const;

    private:
        static std::string texturesPath;
        static ResourceHolder<Texture, std::string> m_bank;

        SDL_Surface* invertPixels(SDL_Surface *src) const;

        GLuint m_id;
        std::string m_name;
        std::string m_path;
};

#endif // TEXTURE_H
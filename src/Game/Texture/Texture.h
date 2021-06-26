#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>

class Texture {
	public:
		Texture(const std::string& texturePath, const std::string& textureName);
        Texture(Texture const& copy);
		~Texture();

        Texture& operator=(Texture const &copy);

        bool load();

        GLuint getID() const;
        std::string getName() const;
        std::string getPath() const;

    private:
        SDL_Surface* invertPixels(SDL_Surface *src) const;

        GLuint m_id;
        std::string m_name;
        std::string m_path;
};

#endif // TEXTURE_H
/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <GL/glew.h>


namespace NoxEngine {

    class TextureLoader;

    class Texture {
        friend class TextureLoader;

        public:
            explicit Texture(const std::string& texturePath, const std::string& textureName);
            explicit Texture(Texture const& copy);

            ~Texture();

            Texture& operator=(Texture const &copy);

            GLuint getID() const noexcept;
            std::string getName() const noexcept;
            std::string getPath() const noexcept;

        private:

            GLuint m_id {0};
            std::string m_name {""};
            std::string m_path {""};
    };

}

#endif // TEXTURE_HPP
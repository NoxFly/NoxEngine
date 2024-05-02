#include "Texture.hpp"

#include <memory>

#include "Console/Console.hpp"


namespace NoxEngine {

    std::string Texture::texturesPath = "./";
    ResourceHolder<Texture, std::string> Texture::m_bank;


    void Texture::setTexturesPath(const std::string& texturesPath) noexcept {
        Texture::texturesPath = ((texturesPath[0] == '/')? "." : "") + texturesPath;
    }

    bool Texture::load(const std::string& textureName, const std::string& texturePath) {
        std::unique_ptr<Texture> tex = std::make_unique<Texture>(texturePath, textureName);

        if(tex->load()) {
            m_bank.set(textureName, std::move(tex));
            return true;
        }

        return false;
    }

    Texture* Texture::get(const std::string& textureName) noexcept {
        return &m_bank.get(textureName);
    }

    Texture::Texture(const std::string& texturePath, const std::string& textureName):
        m_id(0),
        m_name(textureName),
        m_path(Texture::texturesPath + texturePath)
    {
        
    }

    Texture::Texture(Texture const& copy):
        m_id(0),
        m_name(copy.getName()),
        m_path(copy.getPath())
    {
        
    }

    Texture::~Texture() {
        glDeleteTextures(1, &m_id);
    }


    Texture& Texture::operator=(Texture const &copy) {
        m_name = copy.getName();
        m_path = copy.getPath();

        return *this;
    }

    GLuint Texture::getID() const noexcept {
        return m_id;
    }

    std::string Texture::getName() const noexcept {
        return m_name;
    }

    std::string Texture::getPath() const noexcept {
        return m_path;
    }

    bool Texture::load() {
        SDL_Surface* image = IMG_Load(m_path.c_str());

        if(image == 0) {
            std::string sdlErrorMsg = SDL_GetError();
            Console::error("Texture::load", "Failed to load the texture : " + sdlErrorMsg);
            return false;
        }

        SDL_Surface* invertedImage = invertPixels(image);
        SDL_FreeSurface(image);

        if(glIsTexture(m_id) == GL_TRUE)
            glDeleteTextures(1, &m_id);

        glGenTextures(1, &m_id);

        glBindTexture(GL_TEXTURE_2D, m_id);

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
            return false;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, invertedImage->w, invertedImage->h, 0, format, GL_UNSIGNED_BYTE, invertedImage->pixels);

        // filters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // unlock
        glBindTexture(GL_TEXTURE_2D, 0);

        SDL_FreeSurface(invertedImage);

        return true;
    }

    SDL_Surface* Texture::invertPixels(SDL_Surface *src) const {
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
/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Texture.hpp"

#include <memory>
#include <filesystem>

#include "Console/Console.hpp"

namespace fs = std::filesystem;


namespace NoxEngine {

    Texture::Texture(const std::string& texturePath, const std::string& textureName):
        m_name(textureName),
        m_path(texturePath)
    {}

    Texture::Texture(Texture const& copy):
        m_name(copy.getName()),
        m_path(copy.getPath()),
        m_id(copy.getID())
    {}

    Texture::~Texture() {
        glDeleteTextures(1, &m_id);
    }


    Texture& Texture::operator=(Texture const &copy) {
        m_name = copy.getName();
        m_path = copy.getPath();
        m_id = copy.getID();

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

}
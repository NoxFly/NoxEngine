#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <string>
#include <vector>
#include <GL/glew.h>

#include "resourceBank.h"
#include "Shader.h"
#include "Texture.h"

class Application;

struct arrayPair {
    int size = 0;
    std::vector<float> array = {};
};

struct Geometry {
    arrayPair vertices;
    arrayPair colors;
    arrayPair textures;

    Geometry() {
        vertices = { 0, {} };
        colors = { 0, {} };
        textures = { 0, {} };
    }

    Geometry(const Geometry& g) {
        vertices = g.vertices;
        colors = g.colors;
        textures = g.textures;
    }

    Geometry& operator=(const Geometry& g) {
        vertices = g.vertices;
        colors = g.colors;
        textures = g.textures;

        return *this;
    }
};

struct Material {
    private:
        Shader* m_shader;
        std::vector<Texture*> m_textures;

    public:
        Material():
            m_shader(0), m_textures{}
        {
            
        }

        Material(const std::string& shaderName, const std::string& textureName) {
            setShader(shaderName);
            setTextures(textureName);
        }

        Material(const std::string& shaderName, const std::vector<std::string>& texturesName) {
            setShader(shaderName);
            setTextures(texturesName);
        }

        Material(const Material& m) {
            m_shader = m.getShader();
            m_textures = m.getTextures();
        }

        Material& operator=(const Material& m) {
            m_shader = m.getShader();
            m_textures = m.getTextures();

            return *this;
        }

        Shader* getShader() const {
            return m_shader;
        }

        std::vector<Texture*> getTextures() const {
            return m_textures;
        }

        void setShader(const std::string& shaderName) {
            if(shadersBank.has(shaderName))
                this->m_shader = &shadersBank.get(shaderName);
        }

        void setTextures(const std::string& textureName) {
            if(texturesBank.has(textureName)) {
                m_textures.clear();
                m_textures.push_back(&texturesBank.get(textureName));
            }
        }

        void setTextures(const std::vector<std::string>& texturesName) {
            m_textures.clear();

            for(const auto& tex : texturesName)
                setTextures(tex);
        }

        bool hasTexture() {
            return m_textures.size() > 0;
        }

        bool hasShader() {
            return m_shader != 0;
        }
};

#endif // GEOMETRY_H
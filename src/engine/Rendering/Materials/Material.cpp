/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Material.hpp"

#include <GL/glew.h>
#include <sstream>
#include <fstream>
#include <unordered_map>

#include "engine/core/Actor/Actor.hpp"

namespace NoxEngine {

    std::shared_ptr<Material> Material::create() {
        return std::shared_ptr<Material>(new Material(Material::s_defaultShader));
    }

    void Material::setDefaultShader(std::shared_ptr<Shader> shader) noexcept {
        s_defaultShader = shader;
    }

    // ------------------- Constructeurs -------------------
    Material::Material(std::shared_ptr<Shader> shader):
        m_shader(shader)
    {}

    Material::Material(std::shared_ptr<Shader> shader, const Color& color):
        m_shader(shader),
        m_diffuse(color)
    {}

    Material::Material(std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture):
        m_shader(shader)
    {
        if(texture) {
            m_textures.push_back(texture);
        }
    }

    Material::Material(std::shared_ptr<Shader> shader, const std::vector<std::shared_ptr<Texture>>& textures, const Color& color):
        m_shader(shader),
        m_textures(textures),
        m_diffuse(color)
    {}

    // ------------------- Setters -------------------
    void Material::setShader(std::shared_ptr<Shader> shader) noexcept {
        if(shader == nullptr) {
            m_shader = shader;
        }
    }

    void Material::setTextures(const std::vector<std::shared_ptr<Texture>>& textures) noexcept {
        m_textures = textures;
    }

    void Material::setDiffuse(const Color& diffuse) noexcept {
        m_diffuse = diffuse;
    }

    void Material::setAmbient(const Color& ambient) noexcept {
        m_ambient = ambient;
    }

    void Material::setSpecular(const Color& specular) noexcept {
        m_specular = specular;
    }

    void Material::setShininess(float shininess) noexcept {
        m_shininess = shininess;
    }

    void Material::setWireframe(bool wireframe) noexcept {
        m_wireframe = wireframe;
    }

    void Material::setOpacity(float opacity) noexcept {
        m_opacity = opacity;
    }


    // ------------------- Getters -------------------
    std::shared_ptr<Shader> Material::getShader() const noexcept {
        return m_shader;
    }

    const std::vector<std::shared_ptr<Texture>>& Material::getTextures() const noexcept {
        return m_textures;
    }

    const Color& Material::getDiffuse() const noexcept {
        return m_diffuse;
    }

    const Color& Material::getAmbient() const noexcept {
        return m_ambient;
    }

    const Color& Material::getSpecular() const noexcept {
        return m_specular;
    }

    float Material::getShininess() const noexcept {
        return m_shininess;
    }

    bool Material::isWireframed() const noexcept {
        return m_wireframe;
    }

    float Material::getOpacity() const noexcept {
        return m_opacity;
    }


    void Material::transferUniforms(Matrices& mvp, const Scene* scene, const V3D& cameraPosition) const {
        if(!m_shader)
            return;

        m_shader->use();

        // Matrices
        m_shader->setMat4("MVP", mvp.get());
        m_shader->setMat4("u_Model", mvp.getModel());
        m_shader->setMat4("u_View", mvp.getView());
        m_shader->setMat4("u_Projection", mvp.getProjection());

        m_shader->setVec3("u_CameraPos", cameraPosition);

        // Material properties
        m_shader->setVec3("u_Diffuse", m_diffuse.vec3());
        m_shader->setVec3("u_Ambient", m_ambient.vec3());
        m_shader->setVec3("u_Specular", m_specular.vec3());
        m_shader->setFloat("u_Shininess", m_shininess);
        m_shader->setFloat("u_Opacity", m_opacity);
        m_shader->setBool("u_Wireframe", m_wireframe);

        // Textures
        for(size_t i = 0; i < m_textures.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(i));
            glBindTexture(GL_TEXTURE_2D, m_textures[i]->getID());
            m_shader->setInt("u_Texture" + std::to_string(i), static_cast<int>(i));
        }

        glActiveTexture(GL_TEXTURE0); // reset


        const auto lights = scene->getLights();

        // sends the lights
        if (lights.size() > 0) {
            // m_shader->setVec3("u_LightPos", lights[0]->getPosition());
            m_shader->setVec3("u_LightColor", lights[0]->getColor().vec3());
            m_shader->setFloat("u_LightPower", lights[0]->getIntensity());
        }
    }

}
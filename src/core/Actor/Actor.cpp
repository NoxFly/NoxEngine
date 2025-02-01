/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Actor.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

#include "utils/utils.hpp"

namespace fs = std::filesystem;

namespace NoxEngine {

    std::string Actor::m_objectsPath = "./";

    void Actor::setObjectsPath(const std::string& objectsPath) noexcept {
        if (!fs::exists(objectsPath) || !fs::is_directory(objectsPath)) {
            Console::error("Actor::setObjectsPath", "The given path is not a valid folder.");
            exit(EXIT_FAILURE);
        }

        Actor::m_objectsPath = ((objectsPath[0] == '/') ? "." : "") + objectsPath;
    }

    Actor::Actor():
        Actor(nullptr, nullptr)
    {}

    Actor::Actor(Geometry* geometry, Material* material):
        m_uuid(generateUUID()),
        m_geometry(geometry), m_material(material),
        m_cullFace(GL_FRONT), m_polygonMode(GL_FILL),
        m_rotation{},
        m_scale{},
        m_hasToTranslate(false), m_hasToRotate(false), m_hasToScale(false)
    {}

    // Actor::Actor(const Actor& copy):
    //     Actor({}, {})
    // {}

    // const Actor& Actor::operator=(const Actor& copy)
    // {}

    Actor::~Actor() {}

    const std::string& Actor::getUUID() const noexcept {
        return m_uuid;
    }

    Geometry* Actor::getGeometry() noexcept {
        return m_geometry;
    }

    Material* Actor::getMaterial() noexcept {
        return m_material;
    }

    const V3D& Actor::getRotation() const noexcept {
        return m_rotation;
    }

    void Actor::setCullFace(const GLenum cullFace) noexcept {
        m_cullFace = cullFace;
    }

    void Actor::setPolygonMode(const GLenum polygonMode) noexcept {
        m_polygonMode = polygonMode;
    }

    void Actor::setPosition(const V3D& position) noexcept {
        this->m_position = position;
        m_hasToTranslate = position.x != 0 || position.y != 0 || position.z != 0;
    }

    void Actor::setRotation(const V3D& rotation) noexcept {
        m_rotation = rotation;
        m_hasToRotate = rotation.x != 0 || rotation.y != 0 || rotation.z != 0;
    }
    
    void Actor::setPosition(const float x, const float y, const float z) noexcept {
        setPosition(V3D(x, y, z));
    }

    void Actor::setRotation(const float rx, const float ry, const float rz) noexcept {
        setRotation(V3D(rx, ry, rz));
    }

    void Actor::rotate(const float x, const float y, const float z) noexcept {
        m_rotation.x += x;
        m_rotation.y += y;
        m_rotation.z += z;
        m_hasToRotate = x != 0.f || y != 0.f || z != 0.f;
    }

    void Actor::scale(const float x, const float y, const float z) noexcept {
        m_scale.x = std::max(0.f, x);
        m_scale.y = std::max(0.f, y);
        m_scale.z = std::max(0.f, z);
        m_hasToScale = m_scale.x != 1.f || m_scale.y != 1.f || m_scale.z != 1.f;
    }

    void Actor::render(Scene* scene, Camera* camera) {
        if(!m_geometry->hasLoaded() || m_material->getShader() == nullptr)
            return;

        auto shader = m_material->getShader();

        Matrices& mvp = camera->getMatrices();

        const bool hasTexture = m_material->getTextures().size() > 0;
        const bool translateOrRotate = m_hasToTranslate || m_hasToRotate || m_hasToScale;

        // apply matrix transformations
        // related to object's position / rotation
        if(translateOrRotate) {
            mvp.push();
            
            if(m_hasToTranslate)
                mvp.translate(this->m_position);

            if(m_hasToRotate)
                mvp.rotate(m_rotation);

            if(m_hasToScale)
                mvp.scale(m_scale);
        }

        // draw
        glPolygonMode(GL_FRONT_AND_BACK, m_material->isWireframed()? GL_LINE : m_polygonMode);
        glCullFace(m_cullFace);

        // lock shader
        shader->use();
            // lock VAO
            glBindVertexArray(m_geometry->getVAO());

                m_material->transferUniforms(mvp, scene);

                if(hasTexture)
                    glBindTexture(GL_TEXTURE_2D, m_material->getTextures()[0]->getID());

                // renders
                glDrawElements(GL_TRIANGLES, m_geometry->getElementCount(), GL_UNSIGNED_SHORT, 0);

                if(hasTexture)
                    glBindTexture(GL_TEXTURE_2D, 0);

            // unlock VAO
            glBindVertexArray(0);
        // unlock shader
        glUseProgram(0);

        // then restore
        if(translateOrRotate)
            mvp.pop();
    }

}
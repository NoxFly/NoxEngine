#include "GL/glew.h"

#include "utils/utils.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace NoxEngine {

    template <Dimension D>
    Actor<D>::Actor():
        Actor(Geometry(), Material())
    {}

    template <Dimension D>
    Actor<D>::Actor(const Geometry& geometry, const Material& material):
        m_uuid(generateUUID()),
        m_dimension(Is2D<D>? 2 : 3), m_is3D(m_dimension == 3),
        m_geometry(geometry), m_material(material),
        m_cullFace(GL_FRONT), m_polygonMode(GL_FILL),
        m_rotation{},
        m_hasToTranslate(false), m_hasToRotate(false)
    {}

    // template <Dimension D>
    // Actor<D>::Actor(const Actor<D>& copy):
    //     Actor<D>({}, {})
    // {}

    // template <Dimension D>
    // const Actor<D>& Actor<D>::operator=(const Actor<D>& copy)
    // {

    // }

    template <Dimension D>
    Actor<D>::~Actor() {
        
    }

    template <Dimension D>
    const std::string& Actor<D>::getUUID() const {
        return m_uuid;
    }

    template <Dimension D>
    Geometry* Actor<D>::getGeometry() {
        return &m_geometry;
    }

    template <Dimension D>
    Material* Actor<D>::getMaterial() {
        return &m_material;
    }

    template <Dimension D>
    const D& Actor<D>::getRotation() const {
        return m_rotation;
    }

    template <Dimension D>
    void Actor<D>::setCullFace(const GLenum cullFace) noexcept {
        m_cullFace = cullFace;
    }

    template <Dimension D>
    void Actor<D>::setPolygonMode(const GLenum polygonMode) noexcept {
        m_polygonMode = polygonMode;
    }

    template <Dimension D>
    void Actor<D>::setPosition(const D& position) noexcept {
        this->m_position = position;
        m_hasToTranslate = position.x != 0 || position.y != 0 || (m_is3D && position.z != 0);
    }

    template <Dimension D>
    void Actor<D>::setRotation(const D& rotation) noexcept {
        m_rotation = rotation;
        m_hasToRotate = rotation.x != 0 || rotation.y != 0 || (m_is3D && rotation.z != 0);
    }
    
    template <Dimension D>
    void Actor<D>::setPosition(const float x, const float y) noexcept requires Is2D<D> {
        setPosition(V2D(x, y));
    }

    template <Dimension D>
    void Actor<D>::setPosition(const float x, const float y, const float z) noexcept requires Is3D<D> {
        setPosition(V3D(x, y, z));
    }

    template <Dimension D>
    void Actor<D>::setRotation(const float rx, const float ry) noexcept requires Is2D<D> {
        setRotation(V2D(rx, ry));
    }

    template <Dimension D>
    void Actor<D>::setRotation(const float rx, const float ry, const float rz) noexcept requires Is3D<D> {
        setRotation(V3D(rx, ry, rz));
    }

    template <Dimension D>
    void Actor<D>::rotate(const float x, const float y) noexcept requires Is2D<D> {
        m_rotation.x += x;
        m_rotation.y += y;
        m_hasToRotate = x != 0 || y != 0;
    }

    template <Dimension D>
    void Actor<D>::rotate(const float x, const float y, const float z) noexcept requires Is3D<D> {
        m_rotation.x += x;
        m_rotation.y += y;
        m_rotation.z += z;
        m_hasToRotate = x != 0 || y != 0 || z != 0;
    }

    template <Dimension D>
    void Actor<D>::render(Scene<D>* scene, Camera<D>* camera) {
        if(!m_geometry.hasLoaded() || m_material.getShader() == nullptr)
            return;

        auto shader = m_material.getShader();
        
        Matrices<D>& mvp = camera->getMatrices();

        const bool hasTexture = m_material.getTextures().size() > 0;
        const bool translateOrRotate = m_hasToTranslate || m_hasToRotate;

        // apply matrix transformations
        // related to object's position / rotation
        if(translateOrRotate) {
            mvp.push();
            
            if(m_hasToTranslate)
                mvp.translate(this->m_position);

            if(m_hasToRotate)
                mvp.rotate(m_rotation);
        }

        // draw
        glPolygonMode(GL_FRONT_AND_BACK, m_material.isWireframed()? GL_LINE : m_polygonMode);
        glCullFace(m_cullFace);

        // lock shader
        shader->use();
            // lock VAO
            glBindVertexArray(m_geometry.getVAO());

                // TODO : fix this issue
                m_material.transferUniforms(
                    mvp.get(), mvp.getModel(), mvp.getView(), mvp.getProjection(),
                    scene->getLights()
                );

                if(hasTexture)
                    glBindTexture(GL_TEXTURE_2D, m_material.getTextures()[0]->getID());

                // renders
                glDrawElements(GL_TRIANGLES, m_geometry.getElementCount_v(), GL_UNSIGNED_SHORT, 0);

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
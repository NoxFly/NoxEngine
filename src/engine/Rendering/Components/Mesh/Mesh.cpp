/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Mesh.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include "engine/core/Actor/Actor.hpp"
#include "engine/Rendering/Geometries/BufferGeometry.hpp"


namespace NoxEngine {

	std::shared_ptr<Mesh> Mesh::create(std::shared_ptr<BufferGeometry> geom, std::shared_ptr<Material> mat) {
		return std::shared_ptr<Mesh>(new Mesh(geom, mat));
	}

	Mesh::Mesh(std::shared_ptr<BufferGeometry> geom, std::shared_ptr<Material> mat):
		m_geometry(std::move(geom)),
		m_material(std::move(mat))
	{}

	void Mesh::setCullFace(const GLenum cullFace) noexcept {
        m_cullFace = cullFace;
    }

    void Mesh::setPolygonMode(const GLenum polygonMode) noexcept {
        m_polygonMode = polygonMode;
    }

	void Mesh::draw(Scene* scene, Matrices& mvp) {
		if(m_geometry == nullptr || m_material == nullptr || !m_geometry->hasPositions() || m_material->getShader() == nullptr)
            return;

		m_geometry->bind();

        const auto shader = m_material->getShader();
        const bool hasTexture = m_material->getTextures().size() > 0;
        

        glPolygonMode(GL_FRONT_AND_BACK, m_material->isWireframed()? GL_LINE : m_polygonMode);
        glCullFace(m_cullFace);

        // lock shader
        shader->use();

		m_material->transferUniforms(mvp, scene);

		// if(hasTexture)
		//     glBindTexture(GL_TEXTURE_2D, m_material->getTextures()[0]->getID());

		m_geometry->draw();

		// if(hasTexture)
		// 	glBindTexture(GL_TEXTURE_2D, 0);

        // unlock shader
        glUseProgram(0);
	}
}
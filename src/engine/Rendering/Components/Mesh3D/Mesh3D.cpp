/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Mesh3D.hpp"

#include <iostream>
#include <fstream>

#include "engine/core/Actor/Actor.hpp"
#include "engine/Rendering/Geometries/BufferGeometry/BufferGeometry.hpp"


namespace NoxEngine {

    Mesh3D::Mesh3D():
		Mesh3D(nullptr, nullptr)
	{}

	Mesh3D::Mesh3D(Geometry* geometry, Material* material):
		m_geometry(geometry),
		m_material(material),
        m_cullFace(GL_BACK),
		m_polygonMode(GL_FILL)
	{

	}

	Geometry* Mesh3D::getGeometry() noexcept {
        return m_geometry;
    }

    Material* Mesh3D::getMaterial() noexcept {
        return m_material;
    }

	void Mesh3D::setCullFace(const GLenum cullFace) noexcept {
        m_cullFace = cullFace;
    }

    void Mesh3D::setPolygonMode(const GLenum polygonMode) noexcept {
        m_polygonMode = polygonMode;
    }

    bool Mesh3D::loadFromFile(const std::string& filepath) {
		const std::string path = Actor::getObjectsPath() + filepath;

		std::ifstream file(path);

		if (!file.is_open() || !file.good()) {
			Console::error("Mesh3D::loadFromFile", "Failed to open file : " + path);
			return false;
		}

		std::string line;

		std::vector<GLfloat> vertices;
		std::vector<GLfloat> normals;
		std::vector<GLfloat> colors;
		std::vector<GLfloat> uvs;
		std::vector<GLushort> vIndices;
		std::vector<GLushort> nIndices;
		std::vector<GLushort> cIndices;
		std::vector<GLushort> uIndices;

		bool vC = false, nC = false, cC = false, uC = false;

		unsigned int lineIndex = 1;

		while(std::getline(file, line)) {
			char header[8];
			auto cline = line.c_str();

			sscanf_s(cline, "%s", header, (unsigned)sizeof(header));

			// comment
			if(strcmp(header, "#") == 0) {
				continue;
			}
			// vertices
			else if(strcmp(header, "v") == 0) {
				float x, y, z;

				sscanf_s(cline, "%f %f %f\n", &x, &y, &z);

				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);
				
				if(!vC) {
					vC = true;
				}
			}
			// normals
			else if(strcmp(header, "vn") == 0) {
				float x, y, z;

				sscanf_s(cline, "%f %f %f\n", &x, &y, &z);

				normals.push_back(x);
				normals.push_back(y);
				normals.push_back(z);

				if (!nC) {
					nC = true;
				}
			}
			// colors
			else if(strcmp(header, "vc") == 0) {
				float r, g, b;
				
				sscanf_s(cline, "%f %f %f\n", &r, &g, &b);
				
				colors.push_back(r);
				colors.push_back(g);
				colors.push_back(b);

				if(!cC) {
					cC = true;
				}
			}
			// uvs
			else if(strcmp(header, "vt") == 0) {
				float x, y;
				
				sscanf_s(cline, "%f %f\n", &x, &y);
				
				uvs.push_back(x);
				uvs.push_back(y);

				if(!uC) {
					uC = true;
				}
			}
			// EBO
			else if(strcmp(header, "f") == 0) {
				for(unsigned int i = 0; i < 3; i++) {
					if(vC) {
						GLushort v;
						
                        std::cout << "cline : " << cline;
						if(!sscanf_s(cline, "%hu", &v)) {
							Console::error("Mesh3D::loadFromFile", "Syntax error on vertex indice (line " + std::to_string(lineIndex) + ")");
							return false;
						}

						vIndices.push_back(v);
					}

					if(nC) {
						GLushort n;

						if(!sscanf_s(cline, "/%hu", &n)) {
							Console::error("Mesh3D::loadFromFile", "Syntax error on normal indice (line " + std::to_string(lineIndex) + ")");
							return false;
						}

						nIndices.push_back(n);
					}

					if(uC) {
						GLushort uv;
						
						if(!sscanf_s(cline, "/%hu", &uv)) {
							Console::error("Mesh3D::loadFromFile", "Syntax error on texture indice (line " + std::to_string(lineIndex) + ")");
							return false;
						}

						uIndices.push_back(uv);
					}

					if(cC) {
						GLushort c;

						if(!sscanf_s(cline, "/%hu", &c)) {
							Console::error("Mesh3D::loadFromFile", "Syntax error on color indice (line " + std::to_string(lineIndex) + ")");
							return false;
						}

						cIndices.push_back(c);
					}
				}
			}

			++lineIndex;
		}

		GeometryData data = {
			vertices,
			normals,
			colors,
			uvs,
			vIndices
		};

        auto geometry = new BufferGeometry();
        
        if(geometry->loadFromData(data)) {
            m_geometry = geometry;
            return true;
        }

		return false;
	}


	void Mesh3D::draw(Scene* scene, Matrices& mvp) {
		std::cout << "Drawing Mesh3D\n";

		if(!m_geometry->hasLoaded() || m_material->getShader() == nullptr)
            return;

        const auto shader = m_material->getShader();
        const bool hasTexture = m_material->getTextures().size() > 0;
        

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

        
	}
}
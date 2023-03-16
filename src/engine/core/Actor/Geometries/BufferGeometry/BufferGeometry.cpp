#pragma warning (disable: 4244 4312 4996 6031 6054)

#include "BufferGeometry.hpp"

#include <fstream>

#include "core/engine.typedef.hpp"
#include "Console/Console.hpp"
#include "utils/utils.hpp"

namespace NoxEngine {

	BufferGeometry::BufferGeometry()
	{

	}

	BufferGeometry::~BufferGeometry() {

	}

	bool BufferGeometry::loadFromData(const GeometryData& data) {
		m_data = data;

		return load();
	}

	bool BufferGeometry::loadFromFile(const std::string& filepath) {
		const std::string path = Geometry::objectsPath + filepath;

		std::ifstream file(path);

		if (!file.is_open() || !file.good()) {
			Console::error("BufferGeometry::loadFromFile", "Failed to open file : " + path);
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

		while (std::getline(file, line)) {
			char header[8];
			auto cline = line.c_str();

			sscanf(cline, "%s", header);

			// comment
			if (strcmp(header, "#") == 0) {
				continue;
			}
			// vertices
			else if (strcmp(header, "v") == 0) {
				float x, y, z;

				sscanf(cline, "%f %f %f\n", &x, &y, &z);

				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);
				
				if (!vC) {
					vC = true;
				}
			}
			// normals
			else if (strcmp(header, "vn") == 0) {
				float x, y, z;

				sscanf(cline, "%f %f %f\n", &x, &y, &z);

				normals.push_back(x);
				normals.push_back(y);
				normals.push_back(z);

				if (!nC) {
					nC = true;
				}
			}
			// colors
			else if (strcmp(header, "vc") == 0) {
				float r, g, b;
				
				sscanf(cline, "%f %f %f\n", &r, &g, &b);
				
				colors.push_back(r);
				colors.push_back(g);
				colors.push_back(b);

				if (!cC) {
					cC = true;
				}
			}
			// uvs
			else if (strcmp(header, "vt") == 0) {
				float x, y;
				
				sscanf(cline, "%f %f\n", &x, &y);
				
				uvs.push_back(x);
				uvs.push_back(y);

				if (!uC) {
					uC = true;
				}
			}
			// EBO
			else if (strcmp(header, "f") == 0) {
				for (unsigned int i = 0; i < 3; i++) {
					if (vC) {
						GLushort v;
						
						if (!sscanf(cline, "%hu", &v)) {
							Console::error("BufferGeometry::loadFromFile", "Syntax error (line " + std::to_string(lineIndex) + ")");
							return false;
						}

						vIndices.push_back(v);
					}

					if (nC) {
						GLushort n;

						if (!sscanf(cline, "/%hu", &n)) {
							Console::error("BufferGeometry::loadFromFile", "Syntax error (line " + std::to_string(lineIndex) + ")");
							return false;
						}

						nIndices.push_back(n);
					}

					if (uC) {
						GLushort uv;
						
						if (!sscanf(cline, "/%hu", &uv)) {
							Console::error("BufferGeometry::loadFromFile", "Syntax error (line " + std::to_string(lineIndex) + ")");
							return false;
						}

						uIndices.push_back(uv);
					}

					if (cC) {
						GLushort c;

						if (!sscanf(cline, "/%hu", &c)) {
							Console::error("BufferGeometry::loadFromFile", "Syntax error (line " + std::to_string(lineIndex) + ")");
							return false;
						}

						cIndices.push_back(c);
					}
				}
			}

			++lineIndex;
		}

		m_data = {
			vertices,
			normals,
			colors,
			uvs,
			vIndices
		};

		return load();
	}

	bool BufferGeometry::load()
	{
		if (m_hasLoaded) {
			// unload previous data
			deleteVAO();
			deleteVBO();
			deleteEBO();

			m_hasLoaded = false;
		}

		const auto fsize = sizeof(float);

		const auto verticesCount	= m_data.vertices.size();
		const auto normalsCount		= m_data.normals.size();
		const auto colorsCount		= m_data.colors.size();
		const auto uvCount			= m_data.uvs.size();
		const auto elementCount		= m_data.indices.size();

		const auto verticesSize		= verticesCount * fsize;
		const auto normalsSize		= normalsCount	* fsize;
		const auto colorsSize		= colorsCount	* fsize;
		const auto uvSize			= uvCount		* fsize;
		const auto elementSize		= elementCount	* fsize;


		m_vertexCount = verticesCount;
		m_elementCount = elementCount;

		const auto totalSize = verticesSize + normalsSize + colorsSize + uvSize;


		// gen VAO, VBO, EBO
		// lock these, and transfer model data to VRAM

		glGenBuffers(1, &m_VBO);
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_EBO);

		if (m_VAO == 0 || m_VBO == 0) {
			Console::error("BufferGeometry::load", "Failed to generate vao/vbo");
			return false;
		}

		// lock VAO/VBO
		glBindVertexArray(m_VAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
				glBufferData(GL_ARRAY_BUFFER, totalSize, nullptr, GL_STATIC_DRAW);

				glBufferSubData(GL_ARRAY_BUFFER, 0, verticesSize, m_data.vertices.data());

				if (normalsCount > 0)
					glBufferSubData(GL_ARRAY_BUFFER, verticesSize, normalsSize, m_data.normals.data());

				if (colorsCount > 0)
					glBufferSubData(GL_ARRAY_BUFFER, verticesSize + normalsSize, colorsSize, m_data.colors.data());

				if (uvCount > 0)
					glBufferSubData(GL_ARRAY_BUFFER, verticesSize + normalsSize + colorsSize, uvSize, m_data.uvs.data());

				// lock layout locations so the program knows which data corresponds to what

				// position attribute
				// glVertexAttribPoint(layoutLocation, bufferCount, float, flase, combien pour faire une ligne, le décalage dans la ligne)
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
				glEnableVertexAttribArray(0); // layout location 0


				// normal attribute
				if (normalsCount > 0) {
					glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)verticesSize);
					glEnableVertexAttribArray(1); // layout location 1
				}

				// color attribute
				if (colorsCount > 0) {
					glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)(verticesSize + normalsSize));
					glEnableVertexAttribArray(2); // layout location 2
				}

				// texture attribute
				if (uvCount > 0) {
					glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)(verticesSize + normalsSize + colorsSize));
					glEnableVertexAttribArray(3); // layout location 3
				}

		// unlock VAO/VBO

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementSize, m_data.indices.data(), GL_STATIC_DRAW);
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		m_hasLoaded = true;

		return true;
	}

}
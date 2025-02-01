/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

//#pragma warning (disable: 4244 4312 4996 6031 6054)

#include "BufferGeometry.hpp"

#include <string>

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
		return load(data);
	}

	bool BufferGeometry::load(const GeometryData& data)
	{
		if(m_hasLoaded) {
			// unload previous data
			deleteVAO();
			deleteVBO();
			deleteEBO();

			m_hasLoaded = false;
		}

		const auto fsize = sizeof(float);

		const auto verticesCount	= data.vertices.size();
		const auto normalsCount		= data.normals.size();
		const auto colorsCount		= data.colors.size();
		const auto uvCount			= data.uvs.size();

		const auto vElementCount	= data.indices.size();

		const auto verticesSize		= verticesCount * fsize;
		const auto normalsSize		= normalsCount	* fsize;
		const auto colorsSize		= colorsCount	* fsize;
		const auto uvSize			= uvCount		* fsize;

		const auto vElementSize		= vElementCount * fsize;


		m_vertexCount	= verticesCount;
		m_vElementCount = vElementCount;

		const auto totalSize = verticesSize + normalsSize + colorsSize + uvSize;


		// gen VAO, VBO, EBO
		// lock these, and transfer model data to VRAM

		glGenBuffers(1, &m_VBO);
		glGenVertexArrays(1, &m_VAO);

		if (m_VAO == 0 || m_VBO == 0) {
			Console::error("BufferGeometry::load", "Failed to generate vao/vbo");
			return false;
		}

		if (m_vElementCount) {
		}

		if (m_vElementCount) {
			glGenBuffers(1, &m_EBO);

			if (m_EBO == 0) {
				Console::error("BufferGeometry::load", "Failed to generate ebo");
				deleteVAO();
				deleteVBO();
				return false;
			}
		}

		// lock VAO/VBO
		glBindVertexArray(m_VAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
				glBufferData(GL_ARRAY_BUFFER, totalSize, nullptr, GL_STATIC_DRAW);

				glBufferSubData(GL_ARRAY_BUFFER, 0, verticesSize, data.vertices.data());

				if (normalsCount > 0)
					glBufferSubData(GL_ARRAY_BUFFER, verticesSize, normalsSize, data.normals.data());

				if (colorsCount > 0)
					glBufferSubData(GL_ARRAY_BUFFER, verticesSize + normalsSize, colorsSize, data.colors.data());

				if (uvCount > 0)
					glBufferSubData(GL_ARRAY_BUFFER, verticesSize + normalsSize + colorsSize, uvSize, data.uvs.data());

				// lock layout locations so the program knows which data corresponds to what

				// position attribute
				// glVertexAttribPoint(layoutLocation, bufferCount, float, flase, combien pour faire une ligne, le d�calage dans la ligne)
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

				// indices
				if (m_vElementCount) {
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, vElementSize, data.indices.data(), GL_STATIC_DRAW);
					//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				}

			// unlock VAO/VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		m_hasLoaded = true;

		return true;
	}

	void BufferGeometry::indexData(
		const GeometryIndexedData& data,
		GeometryData& out
	) {

		// step 1 : duplicate data by their array of indices

		std::vector<V3D> tempV;
		std::vector<V2D> tempUV;
		std::vector<V3D> tempN;
		std::vector<V3D> tempC;

		const unsigned int count = data.indexes.size();

		for (unsigned int i = 0; i < count; i++) {

			// Get the indices of its attributes
			const auto indices = data.indexes[i];
			// indices[0] = vertex
			// indices[1] = uv
			// indices[2] = normal
			// indices[3] = color
			// if equal to 0, does not exist
			// > 0 = the indice
			// vertex indice should never be equal to 0

			// Get the attributes thanks to the index

			// vertex
			tempV.push_back(data.vertices[indices[0] - 1]);

			// uv
			if (indices[1] > 0) {
				tempUV.push_back(data.uvs[indices[1] - 1]);
			}

			// normal
			if (indices[2] > 0) {
				tempN.push_back(data.normals[indices[2] - 1]);
			}

			// color
			if (indices[3] > 0) {
				tempC.push_back(data.colors[indices[3] - 1]);
			}

		}



		// step 2 : remove duplicate combinations (vert/uv/norm/col)

		const unsigned int vSize = tempV.size();
		const unsigned int uvSize = tempUV.size();
		const unsigned int nSize = tempN.size();
		const unsigned int cSize = tempC.size();

		if (vSize == 0
			|| (uvSize > 0 && uvSize != vSize)
			|| (nSize  > 0 && nSize  != vSize)
			|| (cSize  > 0 && cSize  != vSize)
		) {
			Console::error("BufferGeometry::indexData", "Misconception of provided data : not same size");
			Console::error(std::to_string(vSize) + ", " + std::to_string(uvSize) + ", " + std::to_string(nSize) + ", " + std::to_string(cSize));
			return;
		}

		std::map<PackedVertex, GLushort> VertexToOutIndex;
		GLushort outVsize = 0;

		// For each input vertex
		for (unsigned int i = 0; i < vSize; i++) {

			PackedVertex packed;

			packed.position = tempV[i];

			if (uvSize > 0) {
				packed.uv = tempUV[i];
			}

			if (nSize > 0) {
				packed.normal = tempN[i];
			}

			if (cSize > 0) {
				packed.color = tempC[i];
			}

			GLushort index;

			const bool found = getSimilarVertexIndex(packed, VertexToOutIndex, index);

			if (found) { // A similar vertex is already in the VBO, use it instead !
				out.indices.push_back(index);
			}
			else { // If not, it needs to be added in the output data.
				out.vertices.push_back(tempV[i].x);
				out.vertices.push_back(tempV[i].y);
				out.vertices.push_back(tempV[i].z);

				if (uvSize > 0) {
					out.uvs.push_back(tempUV[i].x);
					out.uvs.push_back(tempUV[i].y);
				}

				if (nSize > 0) {
					out.normals.push_back(tempN[i].x);
					out.normals.push_back(tempN[i].y);
					out.normals.push_back(tempN[i].z);
				}
				
				if (cSize > 0) {
					out.colors.push_back(tempC[i].x);
					out.colors.push_back(tempC[i].y);
					out.colors.push_back(tempC[i].y);
				}

				GLushort newindex = outVsize++;

				out.indices.push_back(newindex);

				VertexToOutIndex[packed] = newindex;
			}
		}

	}

	bool BufferGeometry::getSimilarVertexIndex(
		PackedVertex& packed,
		std::map<PackedVertex, GLushort>& vertexToOutIndex,
		GLushort& result
	) {
		typename std::map<PackedVertex, GLushort>::iterator it = vertexToOutIndex.find(packed);

		if (it == vertexToOutIndex.end()) {
			return false;
		}
		else {
			result = it->second;
			return true;
		}
	}


}
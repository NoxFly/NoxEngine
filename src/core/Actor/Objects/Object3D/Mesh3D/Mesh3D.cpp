/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Mesh3D.hpp"

#include <fstream>

#include "core/Actor/Actor.hpp"
#include "core/Actor/Geometries/BufferGeometry/BufferGeometry.hpp"


namespace NoxEngine {

    Mesh3D::Mesh3D():
        Object3D()
    {}

    Mesh3D::~Mesh3D() {}

    bool Mesh3D::loadFromFile(const std::string& filepath) {
		const std::string path = Actor::m_objectsPath + filepath;

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

			sscanf(cline, "%s", header);

			// comment
			if(strcmp(header, "#") == 0) {
				continue;
			}
			// vertices
			else if(strcmp(header, "v") == 0) {
				float x, y, z;

				sscanf(cline, "%f %f %f\n", &x, &y, &z);

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

				sscanf(cline, "%f %f %f\n", &x, &y, &z);

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
				
				sscanf(cline, "%f %f %f\n", &r, &g, &b);
				
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
				
				sscanf(cline, "%f %f\n", &x, &y);
				
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
						if(!sscanf(cline, "%hu", &v)) {
							Console::error("Mesh3D::loadFromFile", "Syntax error on vertex indice (line " + std::to_string(lineIndex) + ")");
							return false;
						}

						vIndices.push_back(v);
					}

					if(nC) {
						GLushort n;

						if(!sscanf(cline, "/%hu", &n)) {
							Console::error("Mesh3D::loadFromFile", "Syntax error on normal indice (line " + std::to_string(lineIndex) + ")");
							return false;
						}

						nIndices.push_back(n);
					}

					if(uC) {
						GLushort uv;
						
						if(!sscanf(cline, "/%hu", &uv)) {
							Console::error("Mesh3D::loadFromFile", "Syntax error on texture indice (line " + std::to_string(lineIndex) + ")");
							return false;
						}

						uIndices.push_back(uv);
					}

					if(cC) {
						GLushort c;

						if(!sscanf(cline, "/%hu", &c)) {
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

        auto geometry = new BufferGeometry<V3D>();
        
        if(geometry->loadFromData(data)) {
            m_geometry = geometry;
            return true;
        }

		return false;
	}

}
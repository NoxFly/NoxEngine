/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "OBJLoader.hpp"

#include <fstream>
#include <sstream>

#include "engine/core/Actor/Actor.hpp"
#include "engine/Loaders/MTLLoader/MTLLoader.hpp"
#include "Console/Console.hpp"


namespace NoxEngine {

    MeshData OBJLoader::load(const std::string& filepath) {
        MeshData result;
        result.geometry = nullptr;
        result.material = nullptr;

		if(filepath.size() < 4 || filepath.substr(filepath.size() - 4) != ".obj") {
			Console::error("OBJLoader::load", "Unsupported file format : " + filepath);
            return result;
        }

        const std::string path = Actor::getObjectsPath() + filepath;

		std::ifstream file(path);

		if (!file.is_open()) {
			Console::error("Mesh::loadFromOBJ", "Failed to open file : " + path);
			return result;
		}

		std::string line;
		std::string mtlFile;

		std::vector<float> positions;
		std::vector<float> normals;
		std::vector<float> uvs;
		std::vector<uint> indices;

		while(std::getline(file, line)) {
			if(line.empty() || line[0] == '#') {
				continue;
			}

			std::stringstream ss(line);
			std::string type;

			ss >> type;

			if(type == "mtllib") {
				ss >> mtlFile;
				continue;
			}
			else if(type == "v") {
				float x, y, z;
				ss >> x >> y >> z;
				positions.push_back(x);
				positions.push_back(y);
				positions.push_back(z);
				continue;
			}
			else if(type == "vn") {
				float x, y, z;
				ss >> x >> y >> z;
				normals.push_back(x);
				normals.push_back(y);
				normals.push_back(z);
				continue;
			}
			else if(type == "vt") {
				float u, v;
				ss >> u >> v;
				uvs.push_back(u);
				uvs.push_back(v);
				continue;
			}
			else if(type == "f") {
				std::string token;

                std::vector<unsigned int> faceIndices;

				while(ss >> token) {
					uint vi = 0, ti = 0, ni = 0;

					if (sscanf(token.c_str(), "%u/%u/%u", &vi, &ti, &ni) == 3) {}
					else if (sscanf(token.c_str(), "%u//%u", &vi, &ni) == 2) {}
					else if (sscanf(token.c_str(), "%u/%u", &vi, &ti) == 2) {}
					else sscanf(token.c_str(), "%u", &vi);

					if(vi) {
						faceIndices.push_back(static_cast<GLushort>(vi - 1));
					}

                    // if face is quad (4 indices), triangulate (0,1,2) + (0,2,3)
                    if (faceIndices.size() == 3) {
                        indices.push_back(faceIndices[0]);
                        indices.push_back(faceIndices[1]);
                        indices.push_back(faceIndices[2]);
                    }
                    else if (faceIndices.size() == 4) {
                        indices.push_back(faceIndices[0]);
                        indices.push_back(faceIndices[1]);
                        indices.push_back(faceIndices[2]);
                        indices.push_back(faceIndices[0]);
                        indices.push_back(faceIndices[2]);
                        indices.push_back(faceIndices[3]);
                    }
                    else if (faceIndices.size() > 4) {
                        // fan triangulation
                        for (size_t i=1;i+1<faceIndices.size();++i) {
                            indices.push_back(faceIndices[0]);
                            indices.push_back(faceIndices[i]);
                            indices.push_back(faceIndices[i+1]);
                        }
                    }
				}

				continue;
			}
		}

        if(positions.empty()) {
            return result;
        }

		auto geometry = BufferGeometry::create();
        geometry->setPositions(positions);

        if(!normals.empty()) {
            geometry->setNormals(normals);
        }

        if(!uvs.empty()) {
            geometry->setUVs(uvs);
        }

        if(!indices.empty()) {
            geometry->setIndices(indices);
        }

        result.geometry = geometry;

        // Load material if any
        if(!mtlFile.empty()) {
            result.material = MTLLoader::load(mtlFile);
        }

        return result;
    }

}
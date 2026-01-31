/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include <fstream>
#include <sstream>
#include <unordered_map>

#include "NoxEngine/Loader/MTLLoader.hpp"
#include "NoxEngine/Actor/Actor.hpp"

namespace NoxEngine {

    std::shared_ptr<Material> MTLLoader::load(const std::string& filepath) {
		std::ifstream file(filepath);

		if (!file.is_open()) {
			Console::error("MTLLoader::load", "Failed to open MTL file : " + filepath);
			return nullptr;
		}

		std::string line;
		MaterialData currentMat;
		std::unordered_map<std::string, MaterialData> materials;

		while(std::getline(file, line)) {
			if(line.empty() || line[0] == '#') {
				continue;
			}

			std::stringstream ss(line);
			std::string type;

			ss >> type;

			if(type == "newmtl") {
				if(!currentMat.name.empty()) {
					materials[currentMat.name] = currentMat;
				}

				ss >> currentMat.name;
                currentMat = MaterialData();
                currentMat.name = ss.str();
				continue;
			}
			else if(type == "Kd") { // Diffuse color
				float r, g, b;
				ss >> r >> g >> b;
				currentMat.diffuse = Color(r,g,b);
			}
			else if(type == "Ka") { // Ambient color
				float r, g, b;
				ss >> r >> g >> b;
				currentMat.ambient = Color(r,g,b);
			}
			else if(type == "Ks") { // Specular color
				float r, g, b;
				ss >> r >> g >> b;
				currentMat.specular = Color(r,g,b);
			}
			else if(type == "Ns") { // Shininess
				ss >> currentMat.shininess;
			}
		}

		if(!currentMat.name.empty()) {
			materials[currentMat.name] = currentMat;
		}

		if(materials.empty()) {
            return nullptr;
        }

        // For simplicity, just take the first material
        const auto& md = materials.begin()->second;

        auto material = Material::create();

        material->setDiffuse(md.diffuse);
        material->setAmbient(md.ambient);
        material->setSpecular(md.specular);
        material->setShininess(md.shininess);

        return material;
    }

}
/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef SHADER_LOADER_HPP
#define SHADER_LOADER_HPP

#include <string>
#include <memory>
#include <unordered_map>

#include "NoxEngine/Rendering/Shader.hpp"

namespace NoxEngine {

    class ShaderLoader {
        public:
            static std::shared_ptr<Shader> load(const std::string& filepath);
            static std::shared_ptr<Shader> get(const std::string& shaderName);

        private:
            inline static std::unordered_map<std::string, std::shared_ptr<Shader>> s_loadedShaders;

            static std::shared_ptr<Shader> create(const std::string& filepath);
            static void readAndPrecomputeFile(const std::string& filepath, std::string& shaderContent);
            static void getDependencyContent(const std::string& dependencyPath, std::string& dependencyContent);
            static bool checkCompileErrors(GLuint& shader, const std::string& type);
            static void compileShader(std::shared_ptr<Shader> shader, GLuint& shaderId, const std::string& type, const std::string& filepath);
    };

}

#endif // SHADER_LOADER_HPP
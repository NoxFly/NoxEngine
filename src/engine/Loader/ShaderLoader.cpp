/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include <fstream>
#include <sstream>
#include <filesystem>

#include "NoxEngine/Loader/ShaderLoader.hpp"
#include "NoxEngine/utils/string.hpp"
#include "internal/Console.hpp"

namespace fs = std::filesystem;

namespace NoxEngine {

    std::shared_ptr<Shader> ShaderLoader::get(const std::string& shaderName) {
        if(s_loadedShaders.find(shaderName) != s_loadedShaders.end()) {
            return s_loadedShaders[shaderName];
        }

        throw std::runtime_error("Shader " + shaderName + " not loaded");
    }

    std::shared_ptr<Shader> ShaderLoader::load(const std::string& filepath) {
        auto filename = basename(filepath);

        if(s_loadedShaders.find(filename) != s_loadedShaders.end()) {
            return s_loadedShaders[filename];
        }

        std::shared_ptr<Shader> shader = nullptr;

        try {
            shader = create(filepath);
            s_loadedShaders[shader->getName()] = shader;
            return shader;
        }
        catch(std::runtime_error) {
            if (shader != nullptr) {
                shader->destroyShader();
            }

            throw;
        }
    }


    std::shared_ptr<Shader> ShaderLoader::create(const std::string& filepath) {
        auto shader = std::make_shared<Shader>(filepath);

        // security : destruction of potential old shader
        shader->destroyShader();

        // compile vertex & fragment
        compileShader(shader, shader->m_vertexID, "VERTEX", shader->m_shaderPath + ".vert");
        compileShader(shader, shader->m_fragmentID, "FRAGMENT", shader->m_shaderPath + ".frag");

        // shader Program
        shader->m_programID = glCreateProgram();
        glAttachShader(shader->m_programID, shader->m_vertexID);
        glAttachShader(shader->m_programID, shader->m_fragmentID);

        // lock shader's entries
        glBindAttribLocation(shader->m_programID, 0, "in_Vertex");
        glBindAttribLocation(shader->m_programID, 1, "in_Normal");
        glBindAttribLocation(shader->m_programID, 2, "in_Color");
        glBindAttribLocation(shader->m_programID, 3, "in_TexCoord0");

        glLinkProgram(shader->m_programID);

        if (!checkCompileErrors(shader->m_programID, "PROGRAM")) {
            glDeleteProgram(shader->m_programID);
            throw std::runtime_error("Failed to link shader program");
        }

        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(shader->m_vertexID);
        glDeleteShader(shader->m_fragmentID);

        return shader;
    }


    void ShaderLoader::compileShader(std::shared_ptr<Shader> shader, GLuint& shaderId, const std::string& type, const std::string& filepath) {
        GLenum shaderType = type == "VERTEX"
            ? GL_VERTEX_SHADER
            : GL_FRAGMENT_SHADER;
        
        std::string shaderCode;

        readAndPrecomputeFile(filepath, shaderCode);

        shaderCode = "#version " + std::to_string(shader->getGLSLversion()) + " core\n" + shaderCode;

        const GLchar* GLshaderCode = shaderCode.c_str();

        // 2. compile shaders
        shaderId = glCreateShader(shaderType);

        if (shaderId == 0) {
            Console::error("ShaderLoader::load", "Cannot create " + type + " shader");
            throw std::runtime_error("Cannot create " + type + " shader");
        }

        glShaderSource(shaderId, 1, &GLshaderCode, NULL);
        glCompileShader(shaderId);

        if (!checkCompileErrors(shaderId, type)) {
            glDeleteShader(shaderId);
            throw std::runtime_error("Failed to compile " + type + " shader");
        }
    }


    void ShaderLoader::readAndPrecomputeFile(const std::string& filepath, std::string& shaderContent) {
        std::ifstream shaderFile;

        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            // open files
            shaderFile.open(filepath);

            if(!shaderFile.is_open()) {
                throw std::runtime_error("Failed to open " + filepath + " file");
            }

            // Désactiver les exceptions pour EOF pendant la lecture
            shaderFile.exceptions(std::ifstream::badbit);

            std::string lineBuffer;

            // ENHANCEMENT : for scaling, could be defined by rules and splitted and managed by an external entity
            const std::string identifier = "#include";
            const auto identifierSize = identifier.size();

            while(std::getline(shaderFile, lineBuffer)) {
                auto identifierIdx = lineBuffer.find(identifier);

                if(identifierIdx != lineBuffer.npos) {
                    lineBuffer.erase(0, identifierIdx + identifierSize);
                    lineBuffer = trim(lineBuffer);

                    // not form of '#include <>' with a character between tags
                    if(lineBuffer.size() < 3 || lineBuffer.front() != '<' || lineBuffer.back() != '>') {
                        throw std::runtime_error("Malformed syntax for include directive");
                    }

                    const std::string depPath = lineBuffer.substr(1, lineBuffer.size() - 2);
                    lineBuffer = "";
                    
                    getDependencyContent(depPath, lineBuffer);
                }

                shaderContent += lineBuffer + '\n';
            }

            // close file handlers
            shaderFile.close();
        }

        catch(std::ifstream::failure const& e) {
            std::string what = e.what();
            
            Console::error(
                "ShaderLoader::compileShader",
                "File stream error: " + what
            );

            if (shaderFile.is_open()) {
                shaderFile.close();
            }

            throw;
        }

        catch(std::runtime_error const& e) {
            std::string what = e.what();

            Console::error(
                "ShaderLoader::compileShader",
                "Failed to read and parse file " + filepath + " : " + what
            );

            if (shaderFile.is_open()) {
                shaderFile.close();
            }

            throw;
        }
    }

    void ShaderLoader::getDependencyContent(const std::string& dependencyPath, std::string& dependencyContent) {
        const std::string depPath = dependencyPath + ".glsl";

        std::ifstream depFile;

        depFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        // open files
        depFile.open(depPath);

        if(!depFile.is_open()) {
            throw std::runtime_error("Failed to import dependency (" + depPath + ")");
        }

        std::stringstream depStream;

        depStream << depFile.rdbuf();

        depFile.close();

        dependencyContent = depStream.str();
    }


    bool ShaderLoader::checkCompileErrors(GLuint& shader, const std::string& type) {
        int success;
        char infoLog[1024];

        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

            if (success != GL_TRUE) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::string msg = infoLog;
                Console::error("ShaderLoader::checkCompileErrors", "shader compilation error of type: " + type + "\n" + msg);
                return false;
            }
        }

        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);

            if (success != GL_TRUE) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::string msg = infoLog;
                Console::error("ShaderLoader::checkCompileErrors", "program linking error of type: " + type + "\n" + msg);
                return false;
            }
        }

        return true;
    }

}
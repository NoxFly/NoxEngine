/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>

#include "NoxEngine/Rendering/Shader.hpp"
#include "internal/Console.hpp"
#include "NoxEngine/utils/string.hpp"

// GLSL versions :
// 1.10, 1.20, 1.30, 1.40, 1.50, 3.30, 4.00, 4.10, 4.20, 4.30, 4.40, 4.50, 4.60, 1.00 ES, 3.00 ES, 3.10 ES, and 3.20 ES


namespace fs = std::filesystem;


namespace NoxEngine {

    // -------- STATIC --------

    void Shader::setDefaultGLSLversion(GLuint glVersion) noexcept {
        if (std::find(s_GLversions.begin(), s_GLversions.end(), glVersion) != std::end(s_GLversions)) {
            s_defaultGLversion = glVersion;
        }
    }

    bool Shader::checkGLversion(GLuint& glVersion) noexcept {
        if (glVersion == 0) {
            if (s_defaultGLversion == 0) {
                Console::warn("Shader::loadFolder", "No default GLSL version specified. Shaders not loaded.");
                return false;
            }

            glVersion = s_defaultGLversion;
        }
        else if (std::find(s_GLversions.begin(), s_GLversions.end(), glVersion) == std::end(s_GLversions)) {
            Console::warn("Shader::loadFolder", "Unknown GLSL version. Shaders not loaded.");
            glVersion = 0;
            return false;
        }

        return true;
    }


    // --------  --------



    Shader::Shader(const std::string& shaderPath) {
        m_glVersion = s_defaultGLversion;
        m_shaderPath = shaderPath;
        m_shaderName = basename(m_shaderPath);
    }

    /* Shader::Shader(Shader const &copy) {
        m_shaderName = copy.getName();
        m_glVersion = copy.getGLversion();

        load();
    }

    Shader& Shader::operator=(Shader const &copy) {
        m_shaderName = copy.getName();
        m_glVersion = copy.getGLSLversion();

        load();

        return *this;
    } */

    Shader::~Shader() {
        destroyShader();
    }

    GLuint Shader::getGLversion() const noexcept {
        return m_glVersion;
    }

    GLuint Shader::getGLSLversion() const noexcept {
        if (m_glVersion < 33) {
            return (m_glVersion - 19) * 10 + 100; // 110, 120, 130, 140, 150
        }

        // >= 330
        return m_glVersion * 10; // OpenGL 4.2 = 42 * 10 = GLSL 420
    }


    const std::string& Shader::getName() noexcept {
        return m_shaderName;
    }


    GLuint Shader::getID() const noexcept {
        return m_programID;
    }


    void Shader::destroyShader() noexcept {
        if (glIsShader(m_vertexID) == GL_TRUE)
            glDeleteShader(m_vertexID);

        if (glIsShader(m_fragmentID) == GL_TRUE)
            glDeleteShader(m_fragmentID);

        if (glIsProgram(m_programID) == GL_TRUE)
            glDeleteProgram(m_programID);
    }


    void Shader::use() noexcept {
        glUseProgram(m_programID);
    }


    void Shader::setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(m_programID, name.c_str()), (int)value);
    }

    void Shader::setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
    }

    void Shader::setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(m_programID, name.c_str()), value);
    }

    void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(m_programID, name.c_str()), 1, glm::value_ptr(value));
    }

    void Shader::setVec2(const std::string& name, float x, float y) const {
        glUniform2f(glGetUniformLocation(m_programID, name.c_str()), x, y);
    }

    void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(m_programID, name.c_str()), 1, glm::value_ptr(value));
    }

    void Shader::setVec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(m_programID, name.c_str()), x, y, z);
    }

    void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(m_programID, name.c_str()), 1, glm::value_ptr(value));
    }

    void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(m_programID, name.c_str()), x, y, z, w);
    }

    void Shader::setMat2(const std::string& name, const glm::mat2& mat) const {
        glUniformMatrix2fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void Shader::setMat3(const std::string& name, const glm::mat3& mat) const {
        glUniformMatrix3fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

}
#include "Shader.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <filesystem>

#include "Console.hpp"
#include "utils.hpp"

// GLSL versions :
// 1.10, 1.20, 1.30, 1.40, 1.50, 3.30, 4.00, 4.10, 4.20, 4.30, 4.40, 4.50, 4.60, 1.00 ES, 3.00 ES, 3.10 ES, and 3.20 ES


namespace fs = std::filesystem;


namespace NoxEngine {

    // -------- STATIC --------

    void Shader::setShadersPath(const std::string& path) {
        m_shadersPath = ((path[0] == '/')? "." : "") + path;
    }

    void Shader::setDefaultGLSLversion(GLuint glVersion) {
        if(std::find(m_GLversions.begin(), m_GLversions.end(), glVersion) != std::end(m_GLversions)) {
            m_defaultGLversion = glVersion;
        }
    }

    bool Shader::checkGLversion(GLuint& glVersion) {
        if(glVersion == 0) {
            if(m_defaultGLversion == 0) {
                Console::warn("Shader::loadFolder", "No default GLSL version specified. Shaders not loaded.");
                return false;
            }

            glVersion = m_defaultGLversion;
        }
        else if(std::find(m_GLversions.begin(), m_GLversions.end(), glVersion) == std::end(m_GLversions)) {
            Console::warn("Shader::loadFolder", "Unknown GLSL version. Shaders not loaded.");
            glVersion = 0;
            return false;
        }

        return true;
    }

    bool Shader::_load(const std::string& shaderName, GLuint glVersion) {
        std::unique_ptr<Shader> shader = std::make_unique<Shader>(glVersion, shaderName);

        const std::string& name = shader->getName();

        if(!Shader::m_bank.has(name) && shader->load()) {
            m_bank.set(name, std::move(shader));
            return true;
        }

        return false;
    }

    bool Shader::load(const std::string& shaderName, GLuint glVersion) {
        return checkGLversion(glVersion) && _load(shaderName, glVersion);
    }


    void Shader::searchShadersRec(const std::string& folderPath, std::vector<std::string>& savedPaths) {
        for(const auto& entry: fs::directory_iterator(folderPath)) {
            const std::string entryPath = entry.path().generic_string();

            if(entry.is_directory()) {
                searchShadersRec(entryPath, savedPaths);
            }
            else if(entry.is_regular_file() && endsWith(entryPath, ".vert")) {
                savedPaths.push_back(pathWithNoExt(entryPath).substr(m_shadersPath.size()));
            }
        }
    }


    void Shader::loadFolder(GLuint glVersion, const std::string& folderPath) {
        if(!checkGLversion(glVersion)) {
            return;
        }

        const std::string path = startsWith(folderPath, m_shadersPath)? folderPath : m_shadersPath + folderPath;

        std::vector<std::string> shaderPaths{};

        searchShadersRec(path, shaderPaths);

#ifdef DEBUG
        unsigned int done = 0, total = shaderPaths.size();

        std::cout << "Loading shaders... " << done << "/" << total << std::flush;
#endif

        for(const auto& shaderPath : shaderPaths) {
#ifdef DEBUG
            std::cout << "\r" << "Loading shaders... " << done << "/" << total << std::flush;

            if(_load(shaderPath, glVersion)) {
                done++;
            }
#else
            _load(shaderPath, glVersion);
#endif
        }

#ifdef DEBUG
        std::cout << "\r" << "Loading shaders... Done (" << done << ")\n" << std::flush;
#endif
    }

    Shader* Shader::get(const std::string& shaderName) {
        return &m_bank.get(shaderName);
    }






    // --------  --------






    Shader::Shader():
        m_glVersion(20),
        m_vertexID(0),
        m_fragmentID(0),
        m_programID(0),
        m_shaderPath(""),
        m_shaderName("unknown")
    {

    }

    Shader::Shader(GLuint glVersion, const std::string& shaderPath):
        Shader()
    {
        m_glVersion = glVersion;
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

    GLuint Shader::getGLversion() const {
        return m_glVersion;
    }

    GLuint Shader::getGLSLversion() const {
        return m_glVersion * 10; // OpenGL 4.2 = 42 * 10 = GLSL 420
    }


    const std::string& Shader::getName() {
        return m_shaderName;
    }


    GLuint Shader::getId() const {
        return m_programID;
    }


    void Shader::destroyShader() {
        if(glIsShader(m_vertexID) == GL_TRUE)
            glDeleteShader(m_vertexID);

        if(glIsShader(m_fragmentID) == GL_TRUE)
            glDeleteShader(m_fragmentID);

        if(glIsProgram(m_programID) == GL_TRUE)
            glDeleteProgram(m_programID);
    }


    void Shader::use() { 
        glUseProgram(m_programID);
    }


    bool Shader::load() {
        // security : destruction of potential old shader
        destroyShader();

        // compile vertex & fragment
        if(!compileShader(m_vertexID, "VERTEX", Shader::m_shadersPath + m_shaderPath + ".vert"))
            return false;

        if(!compileShader(m_fragmentID, "FRAGMENT", Shader::m_shadersPath + m_shaderPath + ".frag"))
            return false;

        // shader Program
        m_programID = glCreateProgram();
        glAttachShader(m_programID, m_vertexID);
        glAttachShader(m_programID, m_fragmentID);

        // lock shader's entries
        glBindAttribLocation(m_programID, 0, "in_Vertex");
        glBindAttribLocation(m_programID, 1, "in_Color");
        glBindAttribLocation(m_programID, 2, "in_TexCoord0");

        glLinkProgram(m_programID);

        if(!checkCompileErrors(m_programID, "PROGRAM")) {
            glDeleteProgram(m_programID);
            return false;
        }

        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(m_vertexID);
        glDeleteShader(m_fragmentID);

        return true;
    }


    bool Shader::compileShader(GLuint& shader, const std::string& type, const std::string& filepath) {
        GLenum shaderType = type == "VERTEX" ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;

        std::string shaderCode;
        std::ifstream shaderFile;

        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            // open files
            shaderFile.open(filepath);

            std::stringstream shaderStream;

            // read file's buffer contents into streams
            shaderStream << shaderFile.rdbuf();

            // close file handlers
            shaderFile.close();

            // convert stream into string
            shaderCode = shaderStream.str();
        }

        catch(std::ifstream::failure const& e)
        {
            Console::error("Shader::compileShader", "Cannot read file");
            return false;
        }

        shaderCode = "#version " + std::to_string(getGLSLversion()) + " core\n" + shaderCode;

        const GLchar* GLshaderCode = shaderCode.c_str();

        // 2. compile shaders
        shader = glCreateShader(shaderType);

        if(shader == 0) {
            Console::error("Shader::load", "Cannot create " + type + " shader");
            return false;
        }

        glShaderSource(shader, 1, &GLshaderCode, NULL);
        glCompileShader(shader);

        if(!checkCompileErrors(shader, type)) {
            glDeleteShader(shader);
            return false;
        }

        return true;
    }


    bool Shader::checkCompileErrors(GLuint& shader, const std::string& type) {
        int success;
        char infoLog[1024];

        if(type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

            if(success != GL_TRUE) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                Console::error("Shader::checkCompileErrors", "shader compilation error of type: " + type + "\n" + infoLog);
                return false;
            }
        }

        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);

            if(success != GL_TRUE) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                Console::error("Shader::checkCompileErrors", "program linking error of type: " + type + "\n" + infoLog);
                return false;
            }
        }

        return true;
    }

}
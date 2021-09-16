#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <mutex>

#include "Console.h"

std::mutex shaderMutex;

std::string Shader::shadersPath = "./";
ResourceHolder<Shader, std::string> Shader::m_bank;

void Shader::setShadersPath(const std::string& shadersPath) {
    Shader::shadersPath = ((shadersPath[0] == '/')? "." : "") + shadersPath;
}


bool Shader::load(const std::string& shaderName, GLuint glVersion) {
    std::unique_ptr<Shader> shader = std::make_unique<Shader>(shaderName, glVersion);

    if(shader->load()) {
        m_bank.set(shaderName, std::move(shader));
        return true;
    }

    return false;
}

Shader* Shader::get(const std::string& shaderName) {
    return &m_bank.get(shaderName);
}



Shader::Shader():
    m_glVersion(20),
    m_vertexID(0),
    m_fragmentID(0),
    m_programID(0),
    m_shaderName("unknown")
{

}

Shader::Shader(const std::string& shaderName, GLuint glVersion):
    Shader()
{
    this->m_glVersion = glVersion;
    this->m_shaderName = shaderName;
}

Shader::Shader(Shader const &copy) {
    m_shaderName = copy.getName();
    m_glVersion = copy.getGLversion();

    load();
}

Shader& Shader::operator=(Shader const &copy) {
    m_shaderName = copy.getName();
    m_glVersion = copy.getGLSLversion();

    load();

    return *this;
}

Shader::~Shader() {
    destroyShader();
}

GLuint Shader::getGLversion() const {
    return m_glVersion;
}

GLuint Shader::getGLSLversion() const {
    return m_glVersion * 10; // OpenGL 4.2 = 42 * 10 = GLSL 420
}


std::string Shader::getName() const {
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
    std::lock_guard<std::mutex> guard(shaderMutex);

    // security : destruction of potential old shader
    destroyShader();

    // compile vertex & fragment
    if(!compileShader(m_vertexID, "VERTEX", Shader::shadersPath + m_shaderName + ".vert"))
		return false;

	if(!compileShader(m_fragmentID, "FRAGMENT", Shader::shadersPath + m_shaderName + ".frag"))
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
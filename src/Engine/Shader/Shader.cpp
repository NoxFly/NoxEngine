#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "Console.h"

unsigned int last_opengl_major_version = -1;
unsigned int last_opengl_minor_version = -1;

unsigned int Shader::version = 110;

std::map<unsigned int, unsigned int> Shader::GLSLversions = {
    { 20, 110 },
    { 21, 120 },
    { 30, 130 },
    { 31, 140 },
    { 32, 150 },
    { 33, 330 },
    { 40, 400 },
    { 41, 410 },
    { 32, 420 },
    { 43, 430 }
};

int Shader::getGLSLVersion(const unsigned int opengl_major_version, const unsigned int opengl_minor_version) {
    const unsigned int id = std::stoi(std::to_string(opengl_major_version) + std::to_string(opengl_minor_version));

    if(Shader::GLSLversions.begin()->first >= id) {
        return 0;
    }

    if(Shader::GLSLversions.end()->first <= id) {
        return Shader::GLSLversions.end()->second;
    }

    int i = 0;
    std::pair<unsigned int, unsigned int> last;

    for(auto v : Shader::GLSLversions) {
        if(v.first == id)
            return v.second;

        if(i > 0 && v.first > id)
            return last.second;

        last = v;
        i++;
    }

    return Shader::GLSLversions.end()->second;
}

void Shader::setGLSLVersion(const unsigned int opengl_major_version, const unsigned int opengl_minor_version) {
    Shader::version = Shader::getGLSLVersion(opengl_major_version, opengl_minor_version);

    if(opengl_major_version == 1 && opengl_minor_version == 1)
        return;

    if((opengl_major_version != last_opengl_major_version || opengl_minor_version != last_opengl_minor_version) && opengl_major_version < 2) {
        std::ostringstream oss;
        oss << "Shaders won't be created due to unsupported OpenGL version (" << opengl_major_version << "." << opengl_minor_version << ")";
        Console::warn("Shader::setGLSLVersion", oss.str());
    }

    last_opengl_major_version = opengl_major_version;
    last_opengl_minor_version = opengl_minor_version;
}

unsigned int Shader::getGLSLversion() {
    return Shader::version;
}


Shader::Shader():
    m_vertexID(0),
    m_fragmentID(0),
    m_programID(0),
    m_vertexSource(),
    m_fragmentSource()
{

}

Shader::Shader(std::string vertexPath, std::string fragmentPath):
    Shader()
{
    m_vertexSource = vertexPath;
    m_fragmentSource = fragmentPath;
}

Shader::~Shader() {
    destroyShader();
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
    if(!compileShader(m_vertexID, "VERTEX", m_vertexSource))
		return false;

	if(!compileShader(m_fragmentID, "FRAGMENT", m_fragmentSource))
		return false;

    // shader Program
    m_programID = glCreateProgram();
    glAttachShader(m_programID, m_vertexID);
    glAttachShader(m_programID, m_fragmentID);
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

bool Shader::compileShader(GLuint& shader, std::string type, std::string const& filepath) {
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

    catch(std::ifstream::failure e)
    {
        Console::error("Shader::compileShader", "Cannot read file");
        return false;
    }

    const GLchar* GLshaderCode = shaderCode.c_str();

    // 2. compile shaders
    shader = (Shader::version == 0) ? glCreateShaderObjectARB(shaderType) : glCreateShader(shaderType);

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

bool Shader::checkCompileErrors(GLuint shader, std::string type) {
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
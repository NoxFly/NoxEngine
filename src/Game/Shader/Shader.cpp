#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <mutex>

#include "Console.h"

std::mutex shaderMutex;

std::map<GLuint, GLuint> Shader::GLSLversions = {
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

std::string Shader::shadersPath = "./";

void Shader::setShadersPath(const std::string& shadersPath) {
    Shader::shadersPath = shadersPath;
}


Shader::Shader():
    vertexID(0),
    fragmentID(0),
    programID(0),
    glVersion(20),
    shaderName("unknown")
{

}

Shader::Shader(const std::string& shaderName, GLuint glVersion):
    Shader()
{
    this->glVersion = glVersion;
    this->shaderName = shaderName;
}

Shader::~Shader() {
    destroyShader();
}

GLuint Shader::getGLSLversion() const {
    const GLuint lastVersion = 43;

    if(glVersion > lastVersion)
        return Shader::GLSLversions[lastVersion];
    
    return Shader::GLSLversions[glVersion];
}

std::string Shader::getName() const {
    return shaderName;
}

GLuint Shader::getId() const {
    return programID;
}

void Shader::destroyShader() {
    if(glIsShader(vertexID) == GL_TRUE)
		glDeleteShader(vertexID);

	if(glIsShader(fragmentID) == GL_TRUE)
		glDeleteShader(fragmentID);

	if(glIsProgram(programID) == GL_TRUE)
		glDeleteProgram(programID);
}

void Shader::use() { 
    glUseProgram(programID);
}

bool Shader::load() {
    std::lock_guard<std::mutex> guard(shaderMutex);

    // security : destruction of potential old shader
    destroyShader();

    // compile vertex & fragment
    if(!compileShader(vertexID, "VERTEX", Shader::shadersPath + shaderName + ".vert"))
		return false;

	if(!compileShader(fragmentID, "FRAGMENT", Shader::shadersPath + shaderName + ".frag"))
		return false;

    // shader Program
    programID = glCreateProgram();
    glAttachShader(programID, vertexID);
    glAttachShader(programID, fragmentID);

    // lock shader's entries
    glBindAttribLocation(programID, 0, "in_Vertex");
    glBindAttribLocation(programID, 1, "in_Color");
    glBindAttribLocation(programID, 2, "in_TexCoord0");

    glLinkProgram(programID);

    if(!checkCompileErrors(programID, "PROGRAM")) {
        glDeleteProgram(programID);
        return false;
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);

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

    catch(std::ifstream::failure e)
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
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <map>
#include <GL/glew.h>

class Shader {
	public:
		Shader();
		Shader(std::string vertexPath, std::string fragmentPath);
		~Shader();

        void use();
        bool load();
        GLuint getProgramID() const;
        
        static void setGLSLVersion(const unsigned int opengl_major_version, const unsigned int opengl_minor_version);
        static unsigned int getGLSLversion();

    protected:
		GLuint m_vertexID, m_fragmentID, m_programID;
		std::string m_vertexSource;
		std::string m_fragmentSource;

        bool checkCompileErrors(GLuint shader, std::string type);
        bool compileShader(GLuint& shader, std::string type, std::string const& filepath);
        void destroyShader();


        static unsigned int version;
        static std::map<unsigned int, unsigned int> GLSLversions;
        static int getGLSLVersion(const unsigned int opengl_major_version, const unsigned int opengl_minor_version);
};

#endif // SHADER_H
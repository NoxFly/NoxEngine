#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <map>
#include <GL/glew.h>

#include "ResourceHolder.hpp"

class Shader {
	public:
        static void setShadersPath(const std::string& shadersPath);
        static bool load(const std::string& shaderName, GLuint glVersion);
        static Shader* get(const std::string& shaderName);

        Shader();
		Shader(const std::string& shaderName, GLuint glVersion);
        Shader(Shader const &copy);
        Shader& operator=(Shader const &copy);
		~Shader();

        bool load();
        void use();

        GLuint getId() const;
        GLuint getGLSLversion() const;
        GLuint getGLversion() const;
        std::string getName() const;

        // utility uniform functions
        void setBool(const std::string& name, bool value) const;  
        void setInt(const std::string& name, int value) const;   
        void setFloat(const std::string& name, float value) const;

    protected:
        static std::string shadersPath;
        static ResourceHolder<Shader, std::string> m_bank;

        bool checkCompileErrors(GLuint& shader, const std::string& type);
        bool compileShader(GLuint& shader, const std::string& type, const std::string& filepath);
        void destroyShader();

        GLuint m_glVersion, m_vertexID, m_fragmentID, m_programID;
		std::string m_shaderName;
};

#endif // SHADER_HPP
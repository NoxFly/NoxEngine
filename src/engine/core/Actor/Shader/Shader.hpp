#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <vector>
#include <array>
#include <map>
#include <GL/glew.h>

#include "ResourceHolder/ResourceHolder.hpp"


namespace NoxEngine {

    class Shader {
        public:
            static void setShadersPath(const std::string& shadersPath);
            static void setDefaultGLSLversion(GLuint glVersion);
            static bool load(const std::string& shaderName, GLuint glVersion = 0);
            static void loadFolder(GLuint glVersion = 0, const std::string& folderPath="");
            static Shader* get(const std::string& shaderName);

            explicit Shader();
            explicit Shader(GLuint glVersion, const std::string& shaderPath);
            
            Shader(Shader const &) = delete;

            Shader& operator=(Shader const &) = delete;

            ~Shader();

            bool load();
            void use();

            GLuint getId() const;
            GLuint getGLSLversion() const;
            GLuint getGLversion() const;
            const std::string& getName();

            // utility uniform functions
            void setBool(const std::string& name, bool value) const;  
            void setInt(const std::string& name, int value) const;   
            void setFloat(const std::string& name, float value) const;

        protected:
            inline static std::string m_shadersPath = "./";
            inline static GLuint m_defaultGLversion = 0;
            inline static ResourceHolder<Shader, std::string> m_bank;

            bool checkCompileErrors(GLuint& shader, const std::string& type);
            bool compileShader(GLuint& shader, const std::string& type, const std::string& filepath);
            void destroyShader();

            GLuint m_glVersion, m_vertexID, m_fragmentID, m_programID;
            std::string m_shaderPath, m_shaderName;

        private:
            inline static std::array<GLuint, 13> m_GLversions = { 11, 12, 13, 14, 15, 33, 40, 41, 42, 43, 44, 45, 46 };

            static void searchShadersRec(const std::string& folderPath, std::vector<std::string>& savedPaths);
            static bool checkGLversion(GLuint& glVersion);
            static bool _load(const std::string& shaderName, GLuint glVersion);
    };

}

#endif // SHADER_HPP
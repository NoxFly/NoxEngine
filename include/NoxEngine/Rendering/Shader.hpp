/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <vector>
#include <array>
#include <map>
#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>


namespace NoxEngine {

    class ShaderLoader;

    class Shader {
        friend class ShaderLoader;

        public:
            static void setDefaultGLSLversion(GLuint glVersion) noexcept;

            explicit Shader() = default;
            explicit Shader(const std::string& shaderPath);
            
            Shader(Shader const &) = delete;

            Shader& operator=(Shader const &) = delete;

            ~Shader();

            void use() noexcept;

            GLuint getID() const noexcept;
            GLuint getGLSLversion() const noexcept;
            GLuint getGLversion() const noexcept;
            const std::string& getName() noexcept;

            // utility uniform functions
            void setBool(const std::string& name, bool value) const;  
            void setInt(const std::string& name, int value) const;   
            void setFloat(const std::string& name, float value) const;
            void setVec2(const std::string& name, const glm::vec2& value) const;
            void setVec2(const std::string& name, float x, float y) const;
            void setVec3(const std::string& name, const glm::vec3& value) const;
            void setVec3(const std::string& name, float x, float y, float z) const;
            void setVec4(const std::string& name, const glm::vec4& value) const;
            void setVec4(const std::string& name, float x, float y, float z, float w) const;
            void setMat2(const std::string& name, const glm::mat2& mat) const;
            void setMat3(const std::string& name, const glm::mat3& mat) const;
            void setMat4(const std::string& name, const glm::mat4& mat) const;

        protected:
            inline static GLuint s_defaultGLversion = 0;

            void destroyShader() noexcept;

            GLuint m_glVersion {20};
            GLuint m_vertexID {0};
            GLuint m_fragmentID {0};
            GLuint m_programID {0};
            
            std::string m_shaderPath {""};
            std::string m_shaderName {"unknown"};

        private:
            inline static std::array<GLuint, 13> s_GLversions = { 11, 12, 13, 14, 15, 33, 40, 41, 42, 43, 44, 45, 46 };

            static bool checkGLversion(GLuint& glVersion) noexcept;
    };

}

#endif // SHADER_HPP
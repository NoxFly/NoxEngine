#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <string>
#include <map>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "Shader.h"
#include "ResourceHolder.hpp"

#ifndef BUFFER_OFFSET
    #define BUFFER_OFFSET(offset) ((char*)NULL + (offset))
#endif

class Drawable {
    public:
        static void setShadersBank(ResourceHolder<Shader, std::string>& shadersBank);

        Drawable();
        Drawable(float* vertices, GLuint verticesSize);
        Drawable(float* vertices, float* colors, GLuint verticesSize);
        virtual ~Drawable();

        virtual void draw(glm::mat4& projection, glm::mat4& modelview);
        void setShader(const std::string& shaderName);
        void setWireframe(const bool wireframeState);

    protected:
        static ResourceHolder<Shader, std::string>* shadersBank;

        void load(float* vertices, float* colors, GLuint verticesSize);

        GLuint VBO, VAO, vertexNumber;
        Shader* shader;
        bool wireframe;
};

#endif // DRAWABLE_H
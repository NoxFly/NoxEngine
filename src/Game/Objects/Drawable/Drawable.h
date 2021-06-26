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

typedef struct geometry_t {
    float *vertices = 0;
    float *colors = 0;
    float *textures = 0;
    GLuint elementCount = 0;

    geometry_t() {
        vertices = 0;
        colors = 0;
        textures = 0;
        elementCount = 0;
    }

    geometry_t(const geometry_t& g) {
        vertices = g.vertices;
        colors = g.colors;
        textures = g.textures;
        elementCount = g.elementCount;
    }

    geometry_t& operator=(const geometry_t& g) {
        vertices = g.vertices;
        colors = g.colors;
        textures = g.textures;
        elementCount = g.elementCount;

        return *this;
    }
} Geometry;

class Drawable {
    public:
        static void setShadersBank(ResourceHolder<Shader, std::string>& shadersBank);

        Drawable();
        Drawable(Drawable const &copy);
        Drawable(Geometry& geometry);
        virtual ~Drawable();

        Drawable& operator=(Drawable const &copy);

        virtual void draw(glm::mat4& MVP);
        void setShader(const std::string& shaderName);
        void setWireframe(const bool wireframeState);

        bool isWireframed() const;
        Geometry getGeometry() const;
        Shader* getShader() const;

    protected:
        static ResourceHolder<Shader, std::string>* shadersBank;

        void load();

        bool m_wireframe;
        Geometry m_geometry;
        Shader* m_shader;
        GLuint m_VBO, m_VAO, m_vertexNumber;
};

#endif // DRAWABLE_H
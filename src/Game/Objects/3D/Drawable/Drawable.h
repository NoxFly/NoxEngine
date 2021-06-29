#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <string>
#include <map>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "Object.h"
#include "geometry.h"
#include "Shader.h"
#include "Texture.h"
#include "ResourceHolder.hpp"
#include "MatricesMVP.h"

#ifndef BUFFER_OFFSET
    #define BUFFER_OFFSET(offset) ((char*)NULL + (offset))
#endif

class Drawable: public Object {
    public:
        Drawable();
        Drawable(Drawable const &copy);
        Drawable(Geometry& geometry, Material& material);
        virtual ~Drawable();

        Drawable& operator=(Drawable const &copy);

        virtual void draw(MatricesMVP& MVP);
        void setShader(const std::string& shaderName);
        void setTexture(const std::string& texName);
        void setWireframe(const bool wireframeState);

        bool isWireframed() const;
        Geometry getGeometry() const;
        Material getMaterial() const;
        Geometry& getGeometry();
        Material& getMaterial();
        Shader* getShader() const;
        Texture* getTexture() const;

    protected:
        Drawable(Geometry& geometry, Material& material, bool hasToLoad);

        void load();

        bool m_wireframe;
        Geometry m_geometry;
        Material m_material;
        GLuint m_VBO, m_VAO, m_vertexNumber;
};

#endif // DRAWABLE_H
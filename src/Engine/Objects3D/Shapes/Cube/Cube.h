#ifndef CUBE_H
#define CUBE_H

#include <string>
#include <glm/glm.hpp>

#include "Mesh.h"
#include "Color.h"

class Cube: public Mesh {
    public:
        Cube(const float size);
        Cube(const float size, const Color& color);
        Cube(const float size, const std::string& shaderName);
        Cube(const float size, const std::string& shaderName, const std::string& textureName);
        Cube(const float size, const std::string& shaderName, const std::string& textureName, const Color& color);
        Cube(const float size, const std::string& shaderName, const Color& color);
        ~Cube();

    protected:

};

#endif // CUBE_H
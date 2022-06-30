#ifndef CUBE_HPP
#define CUBE_HPP

#include <string>
#include <glm/glm.hpp>

#include "Mesh.hpp"
#include "Color.hpp"

class Cube: public Mesh {
    public:
        Cube(const float size);
        Cube(const float size, const Color& color);
        Cube(const float size, const std::string& textureName);
        Cube(const float size, const std::string& textureName, const Color& color);
        ~Cube();

    protected:

};

#endif // CUBE_HPP
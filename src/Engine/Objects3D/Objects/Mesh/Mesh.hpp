#ifndef MESH_HPP
#define MESH_HPP

#include "Object.hpp"
#include "Geometry.hpp"
#include "Material.hpp"
#include "MatricesMVP.hpp"


#ifndef BUFFER_OFFSET
    #define BUFFER_OFFSET(offset) ((char*)NULL + (offset))
#endif

class Mesh: public Object {
    public:
        Mesh(const Geometry& geometry, const Material& material);
        ~Mesh();

        bool load();
        
    protected:
        void draw(MatricesMVP* mvp);
};

#endif // MESH_HPP
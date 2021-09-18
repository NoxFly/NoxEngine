#ifndef MESH_H
#define MESH_H

#include "Object.h"
#include "Geometry.h"
#include "Material.h"
#include "MatricesMVP.h"


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

#endif // MESH_H
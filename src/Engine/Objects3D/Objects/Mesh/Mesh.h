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
        Mesh(Geometry& geometry, Material& material);
        ~Mesh();

    protected:
        bool load();
        void draw(MatricesMVP* mvp);
};

#endif // MESH_H
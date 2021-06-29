#ifndef OBJECT_H
#define OBJECT_H

#include "MatricesMVP.h"

class Object {
    public:
        virtual void draw(MatricesMVP& MVP) = 0;
};

#endif // OBJECT_H
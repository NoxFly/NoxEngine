#ifndef OBJECT_H
#define OBJECT_H

#include "utils.h"
#include <string>

class Object {
    public:
        virtual ~Object() {};

        std::string getUUID() { return m_uuid; }


    protected:
        std::string m_uuid = generateUUID();
};

#endif // OBJECT_H
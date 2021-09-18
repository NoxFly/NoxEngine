#include "Object.h"

Object::Object(const Geometry& geometry, const Material& material):
    Object3D(0, 0, 0),
    m_geometry(geometry), m_material(material), m_VBO(0), m_VAO(0), m_vertexNumber(0)
{

}

Object::~Object() {

}

std::string Object::getUUID() {
    return m_uuid;
}

Geometry* Object::getGeometry() {
    return &m_geometry;
}

Material* Object::getMaterial() {
    return &m_material;
};

bool Object::load() {
    return false;
}

void Object::render(MatricesMVP* mvp) {
    // apply matrix transformations
    // related to object's position / rotation
    if(m_hasToTranslate || m_hasToRotate) {
        mvp->push();
        
        if(m_hasToTranslate)
            mvp->translate(m_position);

        if(m_hasToRotate)
            mvp->rotate(m_rotation);
    }

    // draw
    draw(mvp);

    // then restore
    if(m_hasToTranslate || m_hasToRotate)
        mvp->pop();
}
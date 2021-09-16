#include "Scene.h"

Scene::Scene():
    m_objects{}
{

}

Scene::~Scene() {
    
}

void Scene::add(Object* object) {
    m_objects.push_back(object);
}

std::vector<Object*> Scene::getObjects() const {
    return m_objects;
}
#include "Scene.h"

Scene::Scene():
    m_objects{}
{

}

Scene::~Scene() {
    
}

void Scene::add(const Object& object) {
    m_objects.push_back(object);
}
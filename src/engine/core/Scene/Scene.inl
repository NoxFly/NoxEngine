namespace NoxEngine {

    template <Dimension D>
    Scene<D>::Scene():
        m_objects{}, m_lights{}
    {}

    template <Dimension D>
    Scene<D>::~Scene()
    {}


    template <Dimension D>
    void Scene<D>::add(std::shared_ptr<Actor<D>> object) noexcept {
        m_objects.push_back(object);
    }

    template <Dimension D>
    void Scene<D>::add(std::shared_ptr<Light> light) noexcept {
        m_lights.push_back(light);
    }

    template <Dimension D>
    std::vector<std::shared_ptr<Actor<D>>> Scene<D>::getActors() const noexcept {
        return m_objects;
    }

    template <Dimension D>
    std::vector<std::shared_ptr<Light>> Scene<D>::getLights() const noexcept {
        return m_lights;
    }


}
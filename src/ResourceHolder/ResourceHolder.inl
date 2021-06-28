template <typename Resource, typename Identifier>
void ResourceHolder<Resource, Identifier>::set(const Identifier id, const Resource& resource) {
    if(!has(id))
        insertResource(id, std::make_unique<Resource>(resource));
}

template <typename Resource, typename Identifier>
void ResourceHolder<Resource, Identifier>::set(const Identifier id, std::unique_ptr<Resource> resource) {
    if(!has(id))
        insertResource(id, std::move(resource));
}

template <typename Resource, typename Identifier>
void ResourceHolder<Resource, Identifier>::remove(const Identifier id) {
    if(has(id))
        m_resourceMap.erase(id);
}

template <typename Resource, typename Identifier>
Resource& ResourceHolder<Resource, Identifier>::get(const Identifier id) {
    return *m_resourceMap.at(id).get();
}

template <typename Resource, typename Identifier>
const Resource& ResourceHolder<Resource, Identifier>::get(const Identifier id) const {
    return *m_resourceMap.at(id).get();
}

template <typename Resource, typename Identifier>
bool ResourceHolder<Resource, Identifier>::has(const Identifier id) const {
    return m_resourceMap.count(id) > 0;
}

template <typename Resource, typename Identifier>
void ResourceHolder<Resource, Identifier>::insertResource(Identifier id, std::unique_ptr<Resource> resource) {
    m_resourceMap.insert(std::make_pair(id, std::move(resource)));
}
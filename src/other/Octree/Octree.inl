template<class T>
Octree<T>::Octree(const OctRect& bounds, unsigned int rootCapacity):
    m_capacity(rootCapacity),
    m_bounds(bounds),
    m_nodes{},
    m_children{ nullptr }
{

}

template<class T>
Octree<T>::Octree(const int capacity, const int x, const int y, const int z, const int w, const int h, const int d):
    Octree<T>(OctRect{ x, y, z, w, h, d }, capacity)
{

}

template<class T>
Octree<T>::~Octree() {
    clear();
}

template<class T>
void Octree<T>::clear() {
    m_nodes.clear();

    if(isDivided()) {
        for(auto& child : m_children) {
            child.get()->clear();
            child.reset();
        }
    }
}

template<class T>
int Octree<T>::getCapacity() const {
    return m_capacity;
}

template<class T>
const OctRect& Octree<T>::getBounds() const {
    return m_bounds;
}

template<class T>
OctRect& Octree<T>::getBounds() {
    return m_bounds;
}

template<class T>
bool Octree<T>::isDivided() const {
    return m_children[0] != nullptr;
}

template<class T>
const std::vector<OctreeNode<T>>& Octree<T>::getNodes() const {
    return m_nodes;
}

template<class T>
std::vector<OctreeNode<T>>& Octree<T>::getNodes() {
    return m_nodes;
}

template<class T>
std::unique_ptr<Octree<T>> Octree<T>::getChildren(unsigned int i) {
    if(isDivided())
        return &m_children[i];
    
    return nullptr;
}


template<class T>
bool Octree<T>::subdivide() {
    if(isDivided())
        return false;

    const unsigned int cap = std::max((unsigned int)1, m_capacity / 2);

    const auto x = m_bounds.x,
        y = m_bounds.y,
        z = m_bounds.z,
        w = m_bounds.w,
        h = m_bounds.h,
        d = m_bounds.d;

    const auto i = w / 2;
    const auto j = h / 2;
    const auto k = d / 2;

    const auto xp = x + i;
    const auto yp = y + j;
    const auto zp = z + k;

    m_children[0].reset(new Octree<T>(cap, x,  y,  z,  i, j, k));
    m_children[1].reset(new Octree<T>(cap, xp, y,  z,  i, j, k));
    m_children[2].reset(new Octree<T>(cap, x,  yp, z,  i, j, k));
    m_children[3].reset(new Octree<T>(cap, xp, yp, z,  i, j, k));
    m_children[4].reset(new Octree<T>(cap, x,  y,  zp, i, j, k));
    m_children[5].reset(new Octree<T>(cap, xp, y,  zp, i, j, k));
    m_children[6].reset(new Octree<T>(cap, x,  yp, zp, i, j, k));
    m_children[7].reset(new Octree<T>(cap, xp, yp, zp, i, j, k));

    return true;
}

template<class T>
bool Octree<T>::push(const glm::vec3& position, T* dataPtr) {
    if(!m_bounds.contains(position))
        return false;

    unsigned int ns = m_nodes.size();

    if(ns < m_capacity) {
        m_nodes.push_back({ position, dataPtr });
        return true;
    }

    if(!isDivided() && !subdivide())
        return false;

    for(const auto& child : m_children) {
        if(child.get()->push(position, dataPtr))
            return true;
    }

    return false;
}


template<class T>
std::unique_ptr<T> Octree<T>::find(const glm::vec3& position) {
    Octree* tree = findTree(position);

    if(tree) {
        for(const auto& p : m_nodes) {
            if(p.position == position)
                return p.dataPtr;
        }
    }

    return nullptr;
}

template<class T>
Octree<T>* Octree<T>::findTree(const glm::vec3& position) {
    if(m_bounds.contains(position)) {
        if(isDivided()) {
            for(const auto& child : m_children) {
                if(child.findTree(position))
                    return child;
            }
        }

        return *this;
    }

    else
        return nullptr;
}

template<class T>
std::vector<T*> Octree<T>::queryRange(const glm::vec3& position, float range) {
    int mr = range * 2;
    OctRect area = {
        (int)(position.x - range),
        (int)(position.y - range),
        (int)(position.z - range),
        mr, mr, mr
    };
    return queryRange(area);
}

template<class T>
std::vector<T*> Octree<T>::queryRange(const OctRect& area) {
    std::vector<T*> nodes = {};

    if(!m_bounds.intersects(area))
        return nodes;

    if(area.wrap(m_bounds))
        return getAllNodesData();
    
    for(auto& n : m_nodes) {
        if(area.contains(n.position))
            nodes.push_back(n.dataPtr);
    }

    if(isDivided()) {
        for(auto& child : m_children) {
            std::vector<T*> nodesChild = child.get()->queryRange(area);
            nodes.insert(nodes.end(), nodesChild.begin(), nodesChild.end());
        }
    }

    return nodes;
}

template<class T>
std::vector<OctreeNode<T>*> Octree<T>::getAllNodes() {
    std::vector<OctreeNode<T>*> nodes = {};

    for(auto& node : m_nodes)
        nodes.push_back(&node);

    if(isDivided()) {
        for(int i=0; i < 8; i++) {
            std::vector<OctreeNode<T>*> nodesChild = m_children[i].get()->getAllNodes();
            nodes.insert(nodes.end(), nodesChild.begin(), nodesChild.end());
        }
    }

    return nodes;
}

template<class T>
std::vector<T*> Octree<T>::getAllNodesData() {
    std::vector<T*> nodes = {};

    for(auto& node : m_nodes)
        nodes.push_back(node.dataPtr);

    if(isDivided()) {
        for(int i=0; i < 8; i++) {
            std::vector<T*> nodesChild = m_children[i].get()->getAllNodesData();
            nodes.insert(nodes.end(), nodesChild.begin(), nodesChild.end());
        }
    }

    return nodes;
}
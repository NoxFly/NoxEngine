#ifndef OCTREE_H
#define OCTREE_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>


template<class T>
struct OctreeNode {
    glm::vec3 position;
    T* dataPtr;
};

struct OctRect {
    int x, y, z, w, h, d;

    bool contains(const glm::vec3& point) const noexcept {
        return (
            x <= point.x && point.x <= x + w &&
            y <= point.y && point.y <= y + h &&
            z <= point.z && point.z <= z + d
        );
    }

    bool intersects(const OctRect& rect) const noexcept {
        return !(
            (x + w < rect.x || rect.x + rect.w < x) ||
            (y + h < rect.y || rect.y + rect.h < y) ||
            (z + d < rect.z || rect.z + rect.d < z)
        );
    }

    bool wrap(const OctRect& rect) const noexcept {
        if(rect.w > w || rect.h > h || rect.d > d)
            return false;

        return (
            contains(glm::vec3(rect.x, rect.y, rect.z)) &&
            contains(glm::vec3(rect.x + rect.w, rect.y + rect.h, rect.z + rect.d))
        );
    }
};

template<class T>
class Octree {
	public:
		explicit Octree<T>(const OctRect& bounds, unsigned int rootCapacity);
		~Octree<T>();

        Octree(const Octree&) = delete;
        Octree& operator=(const Octree&) = delete;

        void clear();

        bool push(const glm::vec3& position, T* dataPtr);
        std::unique_ptr<T> find(const glm::vec3& position);
        Octree<T>* findTree(const glm::vec3& position);
        std::vector<OctreeNode<T>*> getAllNodes();
        std::vector<T*> getAllNodesData();
        std::vector<T*> queryRange(const glm::vec3& position, float range);
        std::vector<T*> queryRange(const OctRect& area);

        int getCapacity() const;
        const OctRect& getBounds() const;
        OctRect& getBounds();
        bool isDivided() const;
        const std::vector<OctreeNode<T>>& getNodes() const;
        std::vector<OctreeNode<T>>& getNodes();
        std::unique_ptr<Octree<T>> getChildren(unsigned int i);

    private:
        Octree<T>(const int capacity, const int x, const int y, const int z, const int w, const int h, const int d);
        
        bool subdivide();

        unsigned int m_capacity; // total nodes that can contains the tree
        OctRect m_bounds; // bounds of the tree
        std::vector<OctreeNode<T>> m_nodes; // elements contained in the node
        std::unique_ptr<Octree<T>> m_children[8]; // children nodes
};

#include "Octree.inl"

#endif // OCTREE_H
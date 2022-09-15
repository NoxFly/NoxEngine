#include "Geometry.hpp"


namespace NoxEngine {

    Geometry::Geometry():
        m_vertices{}, m_colors{}, m_textures{}
    {}

    Geometry::Geometry(const std::vector<float> vertices):
        m_vertices{ vertices }, m_colors{}, m_textures{}
    {}

    Geometry::Geometry(const std::vector<float> vertices, const std::vector<float> colors):
        m_vertices{ vertices }, m_colors{ colors }, m_textures{}
    {}

    Geometry::Geometry(const std::vector<float> vertices, const std::vector<float> colors, const std::vector<float> textures):
        m_vertices{ vertices }, m_colors{ colors }, m_textures{ textures }
    {}

    Geometry::~Geometry() {}


    void Geometry::setVertices(const std::vector<float> vertices) {
        m_vertices = vertices;
    }

    void Geometry::setColors(const std::vector<float> colors) {
        m_colors = colors;
    }

    void Geometry::setTextures(const std::vector<float> textures) {
        m_textures = textures;
    }


    std::vector<float> Geometry::getVertices() const {
        return m_vertices;
    }

    std::vector<float> Geometry::getColors() const {
        return m_colors;
    }

    std::vector<float> Geometry::getTextures() const {
        return m_textures;
    }

}
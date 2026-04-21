// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/GPUInstancing.hpp>
#include <NoxEngine/scene/Geometry.hpp>
#include <NoxEngine/scene/Material.hpp>

#include <GL/glew.h>

namespace Nox {

    InstancedMesh::InstancedMesh(std::shared_ptr<Geometry> geometry,
                                  std::shared_ptr<Material> material,
                                  std::vector<Math::Mat4> transforms)
        : SceneObject("InstancedMesh", SceneObjectType::Mesh)
        , geometry_(std::move(geometry))
        , material_(std::move(material))
        , transforms_(std::move(transforms))
    {}

    InstancedMesh::~InstancedMesh() {
        if (gpuData_.vao != 0) { glDeleteVertexArrays(1, &gpuData_.vao); }
        if (gpuData_.vbo != 0) { glDeleteBuffers(1, &gpuData_.vbo); }
        if (gpuData_.ibo != 0) { glDeleteBuffers(1, &gpuData_.ibo); }
        if (gpuData_.instanceVBO != 0) { glDeleteBuffers(1, &gpuData_.instanceVBO); }
    }

    void InstancedMesh::setTransforms(std::vector<Math::Mat4> transforms) {
        transforms_ = std::move(transforms);
        dirty_ = true;
    }

    void InstancedMesh::addTransform(const Math::Mat4& transform) {
        transforms_.push_back(transform);
        dirty_ = true;
    }

    void InstancedMesh::clearTransforms() {
        transforms_.clear();
        dirty_ = true;
    }

} // namespace Nox

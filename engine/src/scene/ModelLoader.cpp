// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/scene/ModelLoader.hpp>
#include <NoxEngine/core/Logger.hpp>
#include <NoxEngine/scene/Geometry.hpp>
#include <NoxEngine/scene/Material.hpp>
#include <NoxEngine/scene/Mesh.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/glm_element_traits.hpp>

namespace Nox {

    namespace {

        std::shared_ptr<SceneNode> processNode(
            const fastgltf::Asset& asset,
            const fastgltf::Node& node,
            const std::vector<std::shared_ptr<Mesh>>& meshes
        ) {
            auto sceneNode = std::make_shared<SceneNode>(std::string(node.name));

            // Apply transform
            if (auto* trs = std::get_if<fastgltf::TRS>(&node.transform)) {
                sceneNode->transform().setPosition({
                    trs->translation[0], trs->translation[1], trs->translation[2]
                });
                sceneNode->transform().setRotation(Math::Quat(
                    trs->rotation[3], trs->rotation[0], trs->rotation[1], trs->rotation[2]
                ));
                sceneNode->transform().setScale({
                    trs->scale[0], trs->scale[1], trs->scale[2]
                });
            }
            else if (auto* mat = std::get_if<fastgltf::math::fmat4x4>(&node.transform)) {
                // Matrix transform — decompose later if needed
                // For now just set position from the matrix translation
                sceneNode->transform().setPosition({
                    (*mat)[3][0], (*mat)[3][1], (*mat)[3][2]
                });
            }

            // If node has a mesh, add it as a component on the node
            if (node.meshIndex.has_value()) {
                auto meshIdx = node.meshIndex.value();
                if (meshIdx < meshes.size()) {
                    sceneNode->addComponent<std::shared_ptr<Mesh>>(meshes[meshIdx]);
                }
            }

            // Process children
            for (auto childIdx : node.children) {
                auto child = processNode(asset, asset.nodes[childIdx], meshes);
                sceneNode->addChild(child);
            }

            return sceneNode;
        }

    } // anonymous namespace

    std::shared_ptr<SceneNode> ModelLoader::load(const std::filesystem::path& path) {
        fastgltf::Parser parser;
        auto data = fastgltf::GltfDataBuffer::FromPath(path);

        if (data.error() != fastgltf::Error::None) {
            NOX_LOG_ERROR("Failed to load glTF file: {}", path.string());
            return nullptr;
        }

        auto type = fastgltf::determineGltfFileType(data.get());
        fastgltf::Expected<fastgltf::Asset> assetResult(fastgltf::Error::None);

        if (type == fastgltf::GltfType::glTF) {
            assetResult = parser.loadGltf(data.get(), path.parent_path());
        }
        else if (type == fastgltf::GltfType::GLB) {
            assetResult = parser.loadGltfBinary(data.get(), path.parent_path());
        }
        else {
            NOX_LOG_ERROR("Unknown glTF file type: {}", path.string());
            return nullptr;
        }

        if (assetResult.error() != fastgltf::Error::None) {
            NOX_LOG_ERROR("Failed to parse glTF: {}", path.string());
            return nullptr;
        }

        auto& asset = assetResult.get();

        // Convert meshes
        std::vector<std::shared_ptr<Mesh>> meshes;
        meshes.reserve(asset.meshes.size());

        for (const auto& gltfMesh : asset.meshes) {
            for (const auto& primitive : gltfMesh.primitives) {
                std::vector<Vertex> vertices;
                std::vector<uint32_t> indices;

                // Positions
                if (auto it = primitive.findAttribute("POSITION"); it != primitive.attributes.end()) {
                    auto& accessor = asset.accessors[it->accessorIndex];
                    vertices.resize(accessor.count);
                    fastgltf::iterateAccessorWithIndex<Math::Vec3>(asset, accessor,
                        [&](Math::Vec3 pos, std::size_t idx) {
                            vertices[idx].position = pos;
                        });
                }

                // Normals
                if (auto it = primitive.findAttribute("NORMAL"); it != primitive.attributes.end()) {
                    auto& accessor = asset.accessors[it->accessorIndex];
                    fastgltf::iterateAccessorWithIndex<Math::Vec3>(asset, accessor,
                        [&](Math::Vec3 norm, std::size_t idx) {
                            vertices[idx].normal = norm;
                        });
                }

                // UVs
                if (auto it = primitive.findAttribute("TEXCOORD_0"); it != primitive.attributes.end()) {
                    auto& accessor = asset.accessors[it->accessorIndex];
                    fastgltf::iterateAccessorWithIndex<Math::Vec2>(asset, accessor,
                        [&](Math::Vec2 uv, std::size_t idx) {
                            vertices[idx].uv = uv;
                        });
                }

                // Indices
                if (primitive.indicesAccessor.has_value()) {
                    auto& accessor = asset.accessors[primitive.indicesAccessor.value()];
                    indices.reserve(accessor.count);
                    fastgltf::iterateAccessor<uint32_t>(asset, accessor,
                        [&](uint32_t index) {
                            indices.push_back(index);
                        });
                }

                auto geometry = Geometry::fromVertices(std::move(vertices), std::move(indices));
                auto material = Material::standard();

                // Try to get material color from glTF
                if (primitive.materialIndex.has_value()) {
                    auto& gltfMat = asset.materials[primitive.materialIndex.value()];
                    auto& pbr = gltfMat.pbrData;
                    material->setColor(Color(
                        pbr.baseColorFactor[0],
                        pbr.baseColorFactor[1],
                        pbr.baseColorFactor[2],
                        pbr.baseColorFactor[3]
                    ));
                    material->setRoughness(pbr.roughnessFactor);
                    material->setMetallic(pbr.metallicFactor);
                }

                meshes.push_back(std::make_shared<Mesh>(geometry, material));
            }
        }

        // Build scene graph
        auto root = std::make_shared<SceneNode>(path.stem().string());

        if (!asset.scenes.empty()) {
            auto& defaultScene = asset.scenes[asset.defaultScene.value_or(0)];
            for (auto nodeIdx : defaultScene.nodeIndices) {
                auto child = processNode(asset, asset.nodes[nodeIdx], meshes);
                root->addChild(child);
            }
        }

        NOX_LOG_INFO("Loaded model: {} ({} meshes)", path.string(), meshes.size());
        return root;
    }

} // namespace Nox

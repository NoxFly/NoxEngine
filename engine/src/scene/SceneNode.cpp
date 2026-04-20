// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/scene/SceneNode.hpp>

namespace Nox {

    SceneNode::SceneNode(std::string name)
        : name_(std::move(name)) {}

    void SceneNode::addChild(std::shared_ptr<SceneNode> child) {
        child->parent_ = shared_from_this();
        children_.push_back(std::move(child));
    }

    void SceneNode::update(float dt, const Math::Mat4& parentWorld) {
        worldMatrix_ = transform_.computeWorld(parentWorld);

        for (auto& child : children_) {
            child->update(dt, worldMatrix_);
        }
    }

    std::shared_ptr<SceneNode> SceneNode::findByName(std::string_view name) {
        if (name_ == name) {
            return shared_from_this();
        }
        for (auto& child : children_) {
            auto found = child->findByName(name);
            if (found) {
                return found;
            }
        }
        return nullptr;
    }

} // namespace Nox

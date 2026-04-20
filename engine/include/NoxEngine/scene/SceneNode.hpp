// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>
#include <NoxEngine/scene/Transform.hpp>

#include <any>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace Nox {

class SceneNode : public std::enable_shared_from_this<SceneNode> {
public:
    explicit SceneNode(std::string name = "Node");
    virtual ~SceneNode() = default;

    SceneNode(const SceneNode&) = delete;
    SceneNode& operator=(const SceneNode&) = delete;
    SceneNode(SceneNode&&) = default;
    SceneNode& operator=(SceneNode&&) = default;

    [[nodiscard]] const std::string& name() const { return name_; }
    void setName(std::string name) { name_ = std::move(name); }

    Transform& transform() { return transform_; }
    [[nodiscard]] const Transform& transform() const { return transform_; }

    void addChild(std::shared_ptr<SceneNode> child);
    [[nodiscard]] const std::vector<std::shared_ptr<SceneNode>>& children() const { return children_; }
    [[nodiscard]] std::shared_ptr<SceneNode> parent() const { return parent_.lock(); }

    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        auto key = std::type_index(typeid(T));
        components_[key] = std::make_any<T>(std::forward<Args>(args)...);
        return std::any_cast<T&>(components_[key]);
    }

    template<typename T>
    [[nodiscard]] T* getComponent() {
        auto it = components_.find(std::type_index(typeid(T)));
        if (it == components_.end()) return nullptr;
        return std::any_cast<T>(&it->second);
    }

    template<typename T>
    [[nodiscard]] const T* getComponent() const {
        auto it = components_.find(std::type_index(typeid(T)));
        if (it == components_.end()) return nullptr;
        return std::any_cast<T>(&it->second);
    }

    virtual void update(float dt, const Math::Mat4& parentWorld = Math::Mat4(1.0f));

    [[nodiscard]] const Math::Mat4& worldMatrix() const { return worldMatrix_; }

private:
    std::string name_;
    Transform   transform_;
    Math::Mat4  worldMatrix_{ 1.0f };

    std::vector<std::shared_ptr<SceneNode>> children_;
    std::weak_ptr<SceneNode>                parent_;

    std::unordered_map<std::type_index, std::any> components_;
};

} // namespace Nox

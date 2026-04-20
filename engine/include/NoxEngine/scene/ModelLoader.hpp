// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/scene/SceneNode.hpp>

#include <filesystem>
#include <memory>

namespace Nox {

    class RHI;

    class ModelLoader {
    public:
        [[nodiscard]] static std::shared_ptr<SceneNode> load(
            const std::filesystem::path& path
        );
    };

} // namespace Nox

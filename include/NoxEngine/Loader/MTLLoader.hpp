/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef MTL_LOADER_HPP
#define MTL_LOADER_HPP

#include <string>
#include <memory>

#include "NoxEngine/Material/Material.hpp"
#include "NoxEngine/utils/Color.hpp"

namespace NoxEngine {

    class MTLLoader {
        public:
            static std::shared_ptr<Material> load(const std::string& filepath);
    };

}

#endif // MTL_LOADER_HPP
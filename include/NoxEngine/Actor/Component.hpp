/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "NoxEngine/engine.typedef.hpp"

namespace NoxEngine {

    class ActorComponent {
        public:
            virtual ~ActorComponent() = default;
    };

}

#endif // COMPONENT_HPP
/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef ACTOR_HPP
#define ACTOR_HPP

#include <type_traits>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
#include <memory>

#include "NoxEngine/engine.typedef.hpp"
#include "NoxEngine/Camera/Matrices.hpp"
#include "NoxEngine/Camera/Camera.hpp"
#include "NoxEngine/Actor/Movable.hpp"
#include "NoxEngine/Actor/Component.hpp"
#include "NoxEngine/Actor/Drawable.hpp"


namespace NoxEngine {

    class Scene;

    class Actor: public Movable<true>, public std::enable_shared_from_this<Actor> {
        public:
            static std::shared_ptr<Actor> create();

            // ---

            virtual ~Actor() = default;

            const std::string& getUUID() const noexcept;

            const V3D& getRotation() const noexcept;

            std::shared_ptr<Actor> setPosition(const V3D& position) noexcept;
            std::shared_ptr<Actor> setRotation(const V3D& rotation) noexcept;

            std::shared_ptr<Actor> setPosition(const float x, const float y, const float z) noexcept;
            std::shared_ptr<Actor> setRotation(const float rx, const float ry, const float rz) noexcept;

            std::shared_ptr<Actor> rotate(const float x, const float y, const float z) noexcept;

            std::shared_ptr<Actor> scale(const float x, const float y, const float z) noexcept;
            std::shared_ptr<Actor> scale(const float scale) noexcept;

            template<typename T, typename = std::enable_if_t<std::is_base_of_v<ActorComponent, T>>, typename... Args>
            std::shared_ptr<T> addComponent(Args&&... args) {
                auto component = std::make_shared<T>(std::forward<Args>(args)...);
                addComponent(component);
                return component;
            }

            std::shared_ptr<ActorComponent> addComponent(std::shared_ptr<ActorComponent> c);

            void render(Scene* scene, Camera* camera);

        protected:
            explicit Actor() = default;

            const std::string m_uuid { generateUUID() };

            std::vector<std::shared_ptr<ActorComponent>> m_components {};
            std::vector<std::shared_ptr<Drawable>> m_drawables {};

            V3D m_rotation {};
            V3D m_scale {};
            bool m_hasToTranslate { false },
                m_hasToRotate { false },
                m_hasToScale { false };

        private:
            explicit Actor(const Actor&) = delete;
            Actor& operator=(const Actor&) = delete;
    };

}

#endif // ACTOR_HPP
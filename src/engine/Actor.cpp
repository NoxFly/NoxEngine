/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

#include "NoxEngine/Actor/Actor.hpp"
#include "internal/string.hpp"

namespace fs = std::filesystem;

namespace NoxEngine {

    std::shared_ptr<Actor> Actor::create() {
        return std::shared_ptr<Actor>(new Actor());
    }


    // --------------------------------------------------------------------------------
    // 

    /**
     * @brief Get the UUID object
     */
    const std::string& Actor::getUUID() const noexcept {
        return m_uuid;
    }

    /**
     * @brief Get the Rotation object 
     */
    const V3D& Actor::getRotation() const noexcept {
        return m_rotation;
    }

    /**
     * @brief Set the Position object
     */
    std::shared_ptr<Actor> Actor::setPosition(const V3D& position) noexcept {
        this->m_position = position;
        m_hasToTranslate = position.x != 0 || position.y != 0 || position.z != 0;
        
        return shared_from_this();
    }

    /**
     * @brief Set the Rotation object
     */
    std::shared_ptr<Actor> Actor::setRotation(const V3D& rotation) noexcept {
        m_rotation = rotation;
        m_hasToRotate = rotation.x != 0 || rotation.y != 0 || rotation.z != 0;
        
        return shared_from_this();
    }
    
    /**
     * @brief Set the Position object
     */
    std::shared_ptr<Actor> Actor::setPosition(const float x, const float y, const float z) noexcept {
        setPosition(V3D(x, y, z));
        return shared_from_this();
    }

    /**
     * @brief Set the Rotation object
     */
    std::shared_ptr<Actor> Actor::setRotation(const float rx, const float ry, const float rz) noexcept {
        setRotation(V3D(rx, ry, rz));
        return shared_from_this();
    }

    /**
     * @brief Rotate the Actor by the given angles
     */
    std::shared_ptr<Actor> Actor::rotate(const float x, const float y, const float z) noexcept {
        m_rotation.x += x;
        m_rotation.y += y;
        m_rotation.z += z;
        m_hasToRotate = x != 0.f || y != 0.f || z != 0.f;
        
        return shared_from_this();
    }

    /**
     * @brief Scale the Actor by the given factors 
     */
    std::shared_ptr<Actor> Actor::scale(const float x, const float y, const float z) noexcept {
        m_scale.x = std::max(0.f, x);
        m_scale.y = std::max(0.f, y);
        m_scale.z = std::max(0.f, z);
        m_hasToScale = m_scale.x != 1.f || m_scale.y != 1.f || m_scale.z != 1.f;
        
        return shared_from_this();
    }

    /**
     * @brief Uniformly scale the Actor by the given factor
     */
    std::shared_ptr<Actor> Actor::scale(const float s) noexcept {
        return scale(s, s, s);
    }
    
    /**
     * @brief Add a Component to the Actor 
     */
    std::shared_ptr<ActorComponent> Actor::addComponent(std::shared_ptr<ActorComponent> component) {
        if (auto mesh = std::dynamic_pointer_cast<Drawable>(component)) {
            m_drawables.push_back(mesh);
        }
        else {
            m_components.push_back(component);
        }

        return component;
    }

    /**
     * @brief Render the Actor
     */
    void Actor::render(Scene* scene, Camera* camera) {
        if(m_drawables.size() > 0) {
            Matrices& mvp = camera->getMatrices();

            const bool translateOrRotate = m_hasToTranslate || m_hasToRotate || m_hasToScale;

            if(translateOrRotate) {
                mvp.push();
                
                if(m_hasToTranslate)
                    mvp.translate(this->m_position);

                if(m_hasToRotate)
                    mvp.rotate(m_rotation);

                if(m_hasToScale)
                    mvp.scale(m_scale);
            }

            for(const auto& component : m_drawables) {
                component->draw(scene, mvp, camera->getPosition());
            }

            if(translateOrRotate) {
                mvp.pop();
            }
        }
    }

}
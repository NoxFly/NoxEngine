/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "NoxEngine/engine.typedef.hpp"
#include "NoxEngine/Actor/Movable.hpp"
#include "NoxEngine/utils/Color.hpp"

namespace NoxEngine {
	
	class Light {
		public:
			template<typename T, typename = std::enable_if_t<std::is_base_of_v<Light, T>>, typename... Args>
            static std::shared_ptr<T> create(Args&&... args) {
                return std::make_shared<T>(std::forward<Args>(args)...);
            }

			explicit Light(const Color color = Color(255, 255, 255), const float intensity = 1.0f);
			virtual ~Light() = default;

			float getIntensity() const noexcept;
			Color getColor() const noexcept;

			std::string getName() const noexcept;

		protected:
			std::string name {"Light"};
			float m_intensity;
			Color m_color;
	};

}

#endif // LIGHT_HPP
/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef QUAT_HPP
#define QUAT_HPP

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

glm::quat rotationBetweenVectors(glm::vec3 start, glm::vec3 dest);

#endif // QUAT_HPP

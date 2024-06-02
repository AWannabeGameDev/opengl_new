#include "utility/transform.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

Transform::Transform() :
	position{}, rotation{}, scale{1.0f, 1.0f, 1.0f}
{}

glm::mat4 Transform::matrix() const
{
	return glm::translate(glm::mat4{1.0f}, position) * glm::toMat4(rotation) * glm::scale(glm::mat4{1.0f}, scale);
}
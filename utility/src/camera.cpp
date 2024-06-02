#include "utility/camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

Camera::Camera(float fov, float aspectRatio, float near, float far) :
	position{0.0f, 0.0f, 0.0f}, 
	rotation{1.0f, 0.0f, 0.0f, 0.0f}, 
	projection{glm::perspective(fov, aspectRatio, near, far)}
{}

glm::mat4 Camera::viewMatrix() const
{
	return glm::toMat4(glm::inverse(rotation)) * glm::translate(glm::mat4{1.0f}, -position);
}

const glm::mat4& Camera::projectionMatrix() const
{
	return projection;
}

const glm::vec3 Camera::right() const
{
	return rotation * glm::vec3{1.0f, 0.0f, 0.0f};
}

const glm::vec3 Camera::up() const
{
	return rotation * glm::vec3{0.0f, 1.0f, 0.0f};
}

const glm::vec3 Camera::behind() const
{
	return rotation * glm::vec3{0.0f, 0.0f, 1.0f};
}

void Camera::rotateGlobal(const glm::vec3& globalAxis, float radians)
{
	rotation = glm::angleAxis(radians, globalAxis) * rotation;
}

void Camera::rotateLocal(const glm::vec3& localAxis, float radians)
{
	rotateGlobal(rotation * localAxis, radians);
}
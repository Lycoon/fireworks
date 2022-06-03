#include "camera.h"

Camera::Camera() 
	: Camera(glm::vec3(0.0, 2.0, -8.0))
{}

Camera::Camera(glm::vec3 position) : 
	position(position),
	direction(0.0, 0.0, 1.0),
	UP(0.0, 1.0, 0.0)
{}

glm::vec3 Camera::getPosition() const
{
	return position;
}

glm::vec3 Camera::getDirection() const
{
	return direction;
}

void Camera::moveForward(float axis)
{
	position += direction * 0.2f * axis;
}

glm::mat4 Camera::getWorldToViewMatrix() const
{
	return glm::lookAt(position, position + direction, UP);
}
#include "Camera.h"

Camera::Camera() : 
	direction(0.0, 0.0, -1.0), 
	UP(0.0, 1.0, 0.0) 
{
}

glm::mat4 Camera::getWorldToViewMatrix() const
{
	return glm::lookAt(position, position + direction, UP);
}
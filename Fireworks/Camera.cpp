#include "camera.h"

float Camera::currentFrame = 0.0f;
float Camera::deltaTime = 0.0f;
float Camera::lastFrame = 0.0f;

void Camera::updateDeltaTime() {
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

Camera::Camera() 
	: Camera(glm::vec3(0.0f, 0.0f, 0.0f))
{}

Camera::Camera(glm::vec3 position) : 
	position(position),
	direction(0.0f, 0.1f, 1.0f),
	UP(0.0f, 1.0f, 0.0f),
	speed(25.0f)
{}

glm::vec3 Camera::getPosition() const
{
	return position;
}

glm::vec3 Camera::getDirection() const
{
	return direction;
}

float Camera::getDeltaTime() {
	return deltaTime;
}

void Camera::setSpeed(float newSpeed)
{
	this->speed = newSpeed;
}

float Camera::getSpeed() const {
	return speed * deltaTime;
}

void Camera::rotate(float yaw, float pitch)
{
	direction.x = cos(glm::radians(yaw));
	direction.z = sin(glm::radians(yaw));
	direction.y = sin(glm::radians(pitch));

	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
}

void Camera::moveUp(float axis)
{
	position += UP * getSpeed() * axis;
}

void Camera::moveLeft(float axis)
{
	position += glm::normalize(glm::cross(direction, UP)) * getSpeed() * axis;
}

void Camera::moveForward(float axis)
{
	position += direction * getSpeed() * axis;
}

glm::mat4 Camera::getWorldToViewMatrix() const
{
	return glm::lookAt(position, position + direction, UP);
}
#pragma once

#include <glm.hpp>
#include <gtx/transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 position);

	glm::mat4 getWorldToViewMatrix() const;
	glm::vec3 getPosition() const;
	glm::vec3 getDirection() const;

	void moveForward(float axis);
	void moveLeft(float axis);
	void rotate(float yaw, float pitch);

	float getSpeed() const;
	void setSpeed(float newSpeed);
	static float getDeltaTime();
	static void updateDeltaTime();

private:
	glm::vec3 position;
	glm::vec3 direction;
	const glm::vec3 UP;
	float speed;

	static float currentFrame;
	static float deltaTime;
	static float lastFrame;
};

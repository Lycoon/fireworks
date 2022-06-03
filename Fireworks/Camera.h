#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 position);

	glm::mat4 getWorldToViewMatrix() const;
	glm::vec3 getPosition() const;
	glm::vec3 getDirection() const;

	void moveForward(float axis);

private:
	glm::vec3 position;
	glm::vec3 direction;
	const glm::vec3 UP;
};

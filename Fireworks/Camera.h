#pragma once

#include <glm.hpp>
#include <gtx/transform.hpp>

class Camera
{
public:
	Camera();
	glm::mat4 getWorldToViewMatrix() const;
private:
	glm::vec3 position;
	glm::vec3 direction;
	const glm::vec3 UP;
};

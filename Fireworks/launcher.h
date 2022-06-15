#pragma once

#include <glm.hpp>
#include <gtx/transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "firework.h"

class Launcher
{
public:
	Launcher();
	Launcher(glm::vec3 position);

	void update();

private:
	std::vector<Firework> launched;
	glm::vec3 position;

	float delay = 2.0f; // in seconds
	float time = 0.0f;
};

#pragma once

#include <glm.hpp>
#include <stdlib.h>
#include <iostream>

const float GRAVITY = 9.81f;

class Firework
{
public:
	Firework();
	Firework(glm::vec3 position, float launchForce);

	unsigned int getId() const;
	void update(float deltaTime);

private:
	glm::vec3 position;
	glm::vec3 velocity;

	unsigned int id;
	float mass;
	float launchForce;

	static unsigned int fireworksCount;
};

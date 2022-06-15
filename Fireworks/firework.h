#pragma once

#include <glm.hpp>
#include <stdlib.h>
#include <iostream>

const float GRAVITY = 9.81f;

class Firework
{
public:
	Firework();
	Firework(glm::vec3 position, float launchForce, float mass);

	unsigned int getId() const;
	void update(float deltaTime);

private:
	glm::vec3 position;
	glm::vec3 velocity;

	float mass;
	float launchForce;
	float launchRandomness;

	unsigned int id;
	static unsigned int fireworksCount;
};

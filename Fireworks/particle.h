#pragma once

#include <glm.hpp>

class Particle {
public:
	Particle() {}
	Particle(
		glm::vec3 position,
		glm::vec3 speed,
		glm::vec3 direction,
		glm::vec4 color,
		float life,
		float size,
		float weight,
		float distance
	) : position(position),
		speed(speed),
		color(color),
		direction(direction),
		life(life),
		size(size),
		weight(weight),
		distance(distance)
	{}

	glm::vec3 position;
	glm::vec3 speed;
	glm::vec3 direction;
	glm::vec4 color;

	float life;
	float size;
	float weight;
	float distance;
};
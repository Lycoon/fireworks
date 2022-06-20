#pragma once

#include <glm.hpp>
#include <gtx/transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <algorithm>

#include "camera.h"

static const unsigned int maxParticles = 100000;
static const float GRAVITY = 9.81f;

struct Particle {
	glm::vec3 pos, speed;
	unsigned char r, g, b, a;
	float size, angle, weight;
	float life;
	float cameraDst;

	bool operator<(const Particle& right) const {
		return this->cameraDst > right.cameraDst;
	}
};

class Launcher
{
public:
	Launcher();
	Launcher(glm::vec3 position);

	void sortParticles();
	int findUnusedParticle();
	void simulate(Camera &camera, GLfloat* particle_position, GLubyte* particle_color);
	void update(Camera &camera, GLfloat* particle_position, GLubyte* particle_color);

	static unsigned int particlesCount;

private:
	std::unique_ptr<Particle[]> particles{ new Particle[maxParticles] };
	int lastUsedId = 0;

	float delay = 0.2f; // in seconds
	float time = 0.0f;
	float spread = 3.5f;
	float launchSpeed = 25.0f;

	glm::vec3 position;
};

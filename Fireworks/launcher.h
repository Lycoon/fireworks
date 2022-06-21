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
	float size;
	float life;
	bool isLaunching;
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

	void spawnParticle(glm::vec3 position, glm::vec3 speed, glm::vec4 color, float size, float life, bool isLaunching);
	void launchFirework();
	void explode(Particle& p);

	void simulate(Camera &camera, GLfloat* particle_position, GLubyte* particle_color);
	void update(Camera &camera, GLfloat* particle_position, GLubyte* particle_color);

	void sortParticles();
	int findUnusedParticle();

	static unsigned int particlesCount;

private:
	std::unique_ptr<Particle[]> particles{ new Particle[maxParticles] };
	int lastUsedId = 0;

	float delay = 0.2f; // in seconds
	float time = 0.0f;

	float explosionSpread = 25.0f;
	float launchSpread = 10.0f;
	float launchSpeed = 70.0f;

	glm::vec3 position;
};

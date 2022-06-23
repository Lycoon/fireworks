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

static const char* explosionSounds[5] = { 
	"sounds/explosion1.wav", 
	"sounds/explosion2.wav", 
	"sounds/explosion3.wav", 
	"sounds/explosion4.wav", 
	"sounds/explosion5.wav" 
};

static const char* launchSounds[13] = {
	"sounds/launch1.wav",
	"sounds/launch2.wav",
	"sounds/launch3.wav",
	"sounds/launch4.wav",
	"sounds/launch5.wav",
	"sounds/launch6.wav",
	"sounds/launch7.wav",
	"sounds/launch8.wav",
	"sounds/launch9.wav",
	"sounds/launch10.wav",
	"sounds/launch11.wav",
	"sounds/launch12.wav",
	"sounds/launch13.wav"
};

struct Particle {
	enum Type { LAUNCHING, SPARKLE, TRAIL, DEAD };

	glm::vec3 pos, speed;
	unsigned char r, g, b, a;
	float size, life, trailTime, cameraDst;
	Type type;

	bool operator<(const Particle& right) const {
		return this->cameraDst > right.cameraDst;
	}
};

class Launcher
{
public:
	Launcher();
	Launcher(glm::vec3 position);

	void renderTrails(Particle& p, float deltaTime);
	void spawnParticle(glm::vec3 position, glm::vec3 speed, glm::vec4 color, float size, float life, Particle::Type type);
	void launchFirework();
	void explode(Particle& p);

	void simulate(Camera &camera, GLfloat* particle_position, GLubyte* particle_color);
	void update(Camera &camera, GLfloat* particle_position, GLubyte* particle_color);

	void sortParticles();
	int findUnusedParticle();

	static unsigned int particlesCount;

private:
	float launchDelay = 0.3f;       /* delay between each launch (seconds) */
	float trailDelay = 0.05f;        /* delay between each trail spawn (seconds) */

	int sparklesPerExplosion = 400; /* Number of sparkles after explosion */
	float sparkleLife = 1.0f;       /* lifetime of a sparkle (seconds) */
	float sparkleSize = 1.3f;

	float trailSize = 1.6f;         /* size of a trail */
	float trailLife = 0.5f;         /* lifetime of a trail (seconds) */

	float rocketSize = 0.9f;        /* size of a rocket */
	float rocketLife = 8.5f;        /* lifetime of a rocket before exploding (seconds) */

	/* Spread radius */
	float explosionSize = 28.0f;    /* size of explosion blow */
	float explosionSpread = 1.0f;   /* spread of explosion blow */
	float launchSpread = 12.0f;     /* spread of launch direction */
	float launchSpeed = 95.0f;      /* speed of launch direction */

	/* Do not touch */
	std::unique_ptr<Particle[]> particles{ new Particle[maxParticles] };
	glm::vec3 position;
	int lastUsedId = 0;
	float launchTime = 0.0f;
	float trailTime = 0.0f;
};

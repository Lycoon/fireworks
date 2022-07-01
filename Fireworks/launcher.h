#pragma once

#include <glm.hpp>
#include <gtx/transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <algorithm>

#include "camera.h"

static const unsigned int maxParticles = 70000;
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
	enum Type { LAUNCHING, SPARKLE, TRAIL, FOUNTAIN, DEAD };

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
	void explode(Particle& p);
	void launchFirework();
	void launchFountain();

	void simulate(Camera &camera, GLfloat* particle_position, GLubyte* particle_color);
	void update(Camera &camera, GLfloat* particle_position, GLubyte* particle_color);

	void sortParticles();
	int findUnusedParticle();

	static unsigned int particlesCount;

private:
	float launchDelay = 0.15f;       /* delay between each launch (seconds) */
	float trailDelay = 0.04f;        /* delay between each trail spawn (seconds) */
	float fountainDelay = 0.05f;

	int sparklesPerExplosion = 300; /* Number of sparkles after explosion */
	float sparkleLife = 0.9f;       /* lifetime of a sparkle (seconds) */
	float sparkleSize = 1.3f;

	float trailSize = 1.6f;         /* size of a trail */
	float trailLife = 0.6f;         /* lifetime of a trail (seconds) */

	float rocketSize = 0.6f;        /* size of a rocket */
	float rocketLife = 5.5f;        /* lifetime of a rocket before exploding (seconds) 8.5f */

	int fountainDensity = 25;
	float fountainSize = 1.5f;
	float fountainLife = 8.0f;
	float fountainSpeed = 50.0f;
	float fountainSpread = 5.0f;

	/* Colors */
	glm::vec3 fountainColor = glm::vec3(255.0f, 247.0f, 0.0f);
	glm::vec3 fountainFade = glm::vec3(204.0f, 51.0f, 0.0f);
	glm::vec3 trailFade = glm::vec3(0.0f, 0.0f, 0.0f);

	/* Spread radius */
	float explosionSize = 30.0f;    /* size of explosion blow */
	float explosionSpread = 10.0f;   /* spread of explosion blow */
	float launchSpread = 40.0f;     /* spread of launch direction */
	float launchSpeed = 160.0f;      /* speed of launch direction */

	/* Do not touch */
	std::unique_ptr<Particle[]> particles{ new Particle[maxParticles] };
	glm::vec3 position;
	int lastUsedId = 0;
	float launchTime = 0.0f, fountainTime = 0.0f;
	float trailTime = 0.0f;
};

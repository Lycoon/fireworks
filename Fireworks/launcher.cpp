#include "launcher.h"
#include <gtx/string_cast.hpp>

float getRandomNumber(float min, float max);

Launcher::Launcher()
	: Launcher(glm::vec3(0.0f, 0.0f, 0.0f))
{}

Launcher::Launcher(glm::vec3 position)
	: position(position)
{
	std::cout << "Spawning launcher..." << std::endl;

	for (int i = 0; i < maxParticles; i++) {
		particles[i].life = -1.0f;
		particles[i].cameraDst = -1.0f;
		particles[i].isLaunching = false;
	}
}

void Launcher::simulate(Camera& camera, GLfloat* particle_position, GLubyte* particle_color)
{
	float deltaTime = Camera::getDeltaTime();
	particlesCount = 0;

	for (int i = 0; i < maxParticles; i++)
	{
		Particle& p = particles[i];

		if (p.life > 0.0f)
		{
			// Alive particle

			p.speed += glm::vec3(0.0f, -GRAVITY, 0.0f) * deltaTime;
			p.pos += p.speed * deltaTime;
			p.cameraDst = glm::distance(p.pos, camera.getPosition());

			// Fill the GPU buffer
			particle_position[4 * particlesCount + 0] = p.pos.x;
			particle_position[4 * particlesCount + 1] = p.pos.y;
			particle_position[4 * particlesCount + 2] = p.pos.z;
			particle_position[4 * particlesCount + 3] = p.size;

			particle_color[4 * particlesCount + 0] = p.r;
			particle_color[4 * particlesCount + 1] = p.g;
			particle_color[4 * particlesCount + 2] = p.b;
			particle_color[4 * particlesCount + 3] = p.a;

			particlesCount++;
			p.life -= deltaTime;
		}
		else
		{
			// Particle just died
			//std::cout << p.isLaunching << " ";

			if (p.isLaunching)
			{
				p.isLaunching = false;
				explode(p);
			}

			p.cameraDst = -1.0f;
		}
	}
}

void Launcher::spawnParticle(glm::vec3 position, glm::vec3 speed, glm::vec4 color, float size, float life, bool isLaunching)
{
	int idx = findUnusedParticle();

	particles[idx].pos = position;
	particles[idx].speed = speed;
	particles[idx].r = color.r;
	particles[idx].g = color.g;
	particles[idx].b = color.b;
	particles[idx].a = color.a;
	particles[idx].size = size;
	particles[idx].life = life;
	particles[idx].isLaunching = isLaunching;
}

void Launcher::explode(Particle &p)
{
	std::cout << "Boom!" << std::endl;
	for (int i = 0; i < 50; i++)
	{
		float randX = getRandomNumber(-explosionSpread, explosionSpread);
		float randY = getRandomNumber(-explosionSpread, explosionSpread);
		float randZ = getRandomNumber(-explosionSpread, explosionSpread);

		spawnParticle(
			p.pos,
			glm::vec3(randX, randY, randZ),
			glm::vec4(p.r, p.g, p.b, p.a),
			1.5f,
			2.0f,
			false
		);
	}
}

void Launcher::launchFirework()
{
	std::cout << "Launching..." << std::endl;
	float randX = getRandomNumber(-launchSpread, launchSpread);
	float randZ = getRandomNumber(-launchSpread, launchSpread);

	spawnParticle(
		position,
		glm::vec3(randX, launchSpeed, randZ),
		glm::vec4((float)(rand() % 256), (float)(rand() % 256), (float)(rand() % 256), (float)((rand() % 256) / 3)),
		1.5f,
		7.0f, // delay before exploding
		true
	);
}

void Launcher::update(Camera& camera, GLfloat* particle_position, GLubyte* particle_color)
{
	float deltaTime = Camera::getDeltaTime();

	time -= deltaTime;
	if (time <= 0)
	{
		// Firing firework
		launchFirework();

		// Resetting for next launch
		time = delay;
	}

	simulate(camera, particle_position, particle_color);
	sortParticles();
}

void Launcher::sortParticles() {
	std::sort(&particles[0], &particles[maxParticles]);
}

int Launcher::findUnusedParticle() {

	for (int i = lastUsedId; i < maxParticles; i++) {
		if (particles[i].life < 0) {
			lastUsedId = i;
			return i;
		}
	}

	for (int i = 0; i < lastUsedId; i++) {
		if (particles[i].life < 0) {
			lastUsedId = i;
			return i;
		}
	}

	return 0;
}

float getRandomNumber(float min, float max)
{
	return ((float(rand()) / float(RAND_MAX)) * (max - min)) + min;
}
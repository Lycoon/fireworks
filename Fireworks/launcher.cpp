#include "launcher.h"

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
	}
}

void Launcher::update(Camera& camera, GLfloat* particle_position, GLubyte* particle_color)
{
	float deltaTime = Camera::getDeltaTime();

	time -= deltaTime;
	if (time <= 0)
	{
		// Firing firework
		for (int i = 0; i < 1; i++)
		{
			std::cout << "Firing firework..." << std::endl;
			int idx = findUnusedParticle();

			float randX = (float)rand() / (float)(RAND_MAX / spread);
			float randZ = (float)rand() / (float)(RAND_MAX / spread);

			// Launching characteristics
			particles[idx].pos = position;
			particles[idx].life = 5.0f;
			particles[idx].speed = glm::vec3(randX, launchSpeed, randZ);
			particles[idx].r = rand() % 256;
			particles[idx].g = rand() % 256;
			particles[idx].b = rand() % 256;
			particles[idx].a = (rand() % 256) / 3;
			particles[idx].size = (rand() % 1000) / 2000.0f + 0.1f;
		}

		// Resetting for next launch
		time = delay;
	}
	
	// Simulating each particule
	/*for (auto it = launched.begin(); it < launched.end(); it++)
	{
		if (it->getId() == 0)
			it->update(deltaTime);
	}*/

	particlesCount = 0;
	for (int i = 0; i < maxParticles; i++) 
	{
		Particle& p = particles[i];

		// If particle still alive
		if (p.life > 0.0f) 
		{
			if (i == 0)
				std::cout << "pos: " << p.pos.x << ", " << p.pos.y << ", " << p.pos.z << std::endl;
			
			p.speed += glm::vec3(0.0f, -GRAVITY, 0.0f) * deltaTime * 0.5f;
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
			p.cameraDst = -1.0f;
	}

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
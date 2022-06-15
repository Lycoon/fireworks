#include "firework.h"

unsigned int Firework::fireworksCount = 0;

Firework::Firework()
{}

Firework::Firework(glm::vec3 position, float launchForce, float mass)
	: position(position), launchForce(launchForce), mass(mass), launchRandomness(2.5f)
{
	id = fireworksCount;
	fireworksCount++;

	float randX = (float)rand() / (float)(RAND_MAX / launchRandomness);
	float randZ = (float)rand() / (float)(RAND_MAX / launchRandomness);

	velocity = glm::vec3(randX, launchForce, randZ);
}

unsigned int Firework::getId() const {
	return id;
}

void Firework::update(float deltaTime) {
	velocity += glm::vec3(0.0, - GRAVITY * mass * deltaTime, 0.0);
	position += velocity * deltaTime;

	std::cout << "x: " << position.x << ", y: " << position.y << ", z: " << position.z << std::endl;
}
#include "firework.h"

unsigned int Firework::fireworksCount = 0;

Firework::Firework()
{}

Firework::Firework(glm::vec3 position, float launchForce)
	: position(position), launchForce(launchForce)
{
	id = fireworksCount;
	fireworksCount++;

	float randX = (float)rand() / (float)(RAND_MAX / 0.3);
	float randZ = (float)rand() / (float)(RAND_MAX / 0.3);

	velocity = glm::vec3(randX, launchForce, randZ);
}

unsigned int Firework::getId() const {
	return id;
}

void Firework::update(float deltaTime) {
	velocity += -GRAVITY * deltaTime;
	position += velocity * deltaTime;

	std::cout << "x: " << position.x << ", y: " << position.y << ", z: " << position.z << std::endl;
}
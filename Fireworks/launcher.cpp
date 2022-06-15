#include "launcher.h"
#include "Camera.h"

Launcher::Launcher()
	: Launcher(glm::vec3(0.0, 0.0, 0.0))
{}

Launcher::Launcher(glm::vec3 position)
{}

void Launcher::update()
{
	float deltaTime = Camera::getDeltaTime();

	time += deltaTime;
	if (time >= delay)
	{
		// Firing firework
		Firework firework(position, 30.0f, 0.8f);
		launched.push_back(firework);

		// Resetting for next launch
		time = 0;
	}
	
	// Updating individual launched fireworks
	for (auto it = launched.begin(); it < launched.end(); it++)
	{
		if (it->getId() == 0)
			it->update(deltaTime);
	}
}
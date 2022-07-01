#include "launcher.h"
#include "irrKlang.h"

#include <gtx/string_cast.hpp>

using namespace irrklang;
ISoundEngine* soundEngine = createIrrKlangDevice();

float getRandomNumber(float min, float max);
glm::vec3 fadeColor(glm::vec3 startColor, glm::vec3 destColor, float ratio);
glm::vec4 getRandomBrightColor();

auto gravityForce = glm::vec3(0.0f, -GRAVITY, 0.0f);
auto noForce = glm::vec3(1.0f, 1.0f, 1.0f);

Launcher::Launcher()
	: Launcher(glm::vec3(0.0f, 0.0f, 0.0f))
{}

Launcher::Launcher(glm::vec3 position)
	: position(position)
{
	for (int i = 0; i < maxParticles; i++) 
	{
		// Initializing default particle values
		particles[i].life = -1.0f;
		particles[i].cameraDst = -1.0f;
		particles[i].trailTime = trailDelay;
		particles[i].type = Particle::Type::DEAD;
	}
}

void Launcher::simulate(Camera& camera, GLfloat* particle_position, GLubyte* particle_color)
{
	float deltaTime = Camera::getDeltaTime();
	particlesCount = 0;

	for (int i = 0; i < maxParticles; i++)
	{
		// Simulating each particule
		Particle& p = particles[i];

		if (p.life > 0.0f)
		{
			// Alive particle
			p.speed += gravityForce * deltaTime;
			p.pos += p.speed * deltaTime;
			p.cameraDst = glm::distance(p.pos, camera.getPosition());

			renderTrails(p, deltaTime);

			// Fill the GPU buffer
			particle_position[4 * particlesCount + 0] = p.pos.x;
			particle_position[4 * particlesCount + 1] = p.pos.y;
			particle_position[4 * particlesCount + 2] = p.pos.z;

			float lifeRatio = 1.0f;
			auto newColor = glm::vec3(p.r, p.g, p.b);
			switch (p.type)
			{
				case Particle::Type::SPARKLE:
					lifeRatio = p.life / sparkleLife;
					break;
				case Particle::Type::TRAIL:
					lifeRatio = p.life / trailLife;
					newColor = fadeColor(newColor, trailFade, lifeRatio);
					break;
				case Particle::Type::FOUNTAIN:
					lifeRatio = p.life / fountainLife;
					newColor = fadeColor(fountainColor, fountainFade, lifeRatio);
					break;
			}
			particle_position[4 * particlesCount + 3] = p.size * lifeRatio;

			particle_color[4 * particlesCount + 0] = newColor.r;
			particle_color[4 * particlesCount + 1] = newColor.g;
			particle_color[4 * particlesCount + 2] = newColor.b;
			particle_color[4 * particlesCount + 3] = p.life;

			particlesCount++;
			p.life -= deltaTime;
			continue;
		}

		// Particle just died
		if (p.type == Particle::Type::LAUNCHING)
			explode(p);

		p.type = Particle::Type::DEAD;
		p.cameraDst = -1.0f;
	}
}



void Launcher::renderTrails(Particle& p, float deltaTime)
{
	if (p.trailTime <= 0 && (p.type == Particle::Type::SPARKLE || p.type == Particle::Type::LAUNCHING))
	{

		spawnParticle(
			p.pos,
			glm::vec3(0.0f),
			glm::vec4(p.r, p.g, p.b, p.a),
			trailSize,
			trailLife,
			Particle::Type::TRAIL
		);
		p.trailTime = trailDelay;
	}
	p.trailTime -= deltaTime;
}

void Launcher::spawnParticle(glm::vec3 position, glm::vec3 speed, glm::vec4 color, float size, float life, Particle::Type type)
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
	particles[idx].type = type;
}

void Launcher::explode(Particle &p)
{
	int randomSound = getRandomNumber(1, 6);
	soundEngine->play2D(explosionSounds[randomSound - 1]);

	float randSize = getRandomNumber(0, explosionSpread);
	for (int i = 0; i < sparklesPerExplosion; i++)
	{
		float randX = getRandomNumber(-1, 2);
		float randY = getRandomNumber(-1, 2);
		float randZ = getRandomNumber(-1, 2);
		float randSpread = getRandomNumber(0, explosionSpread);
		float randLife = getRandomNumber(0, 1.5f);

		spawnParticle(
			p.pos,
			glm::normalize(glm::vec3(randX, randY, randZ)) * (explosionSize - randSpread + randSize),
			glm::vec4(p.r, p.g, p.b, p.a),
			sparkleSize,
			sparkleLife + randLife,
			Particle::Type::SPARKLE
		);
	}
}

void Launcher::launchFirework()
{
	float randX = getRandomNumber(-launchSpread, launchSpread);
	float randZ = getRandomNumber(-launchSpread, launchSpread);
	float rand = getRandomNumber(-150.0f, 150.0f);

	spawnParticle(
		position + glm::vec3(rand, 0.0, rand),
		glm::vec3(randX, getRandomNumber(launchSpeed, launchSpeed + 25), randZ),
		getRandomBrightColor(),
		rocketSize,
		rocketLife,
		Particle::Type::LAUNCHING
	);
}

void Launcher::launchFountain()
{

	for (int i = 0; i < fountainDensity; i++)
	{
		float randX = getRandomNumber(-fountainSpread, fountainSpread);
		float randZ = getRandomNumber(-fountainSpread, fountainSpread);
		float randY = getRandomNumber(-10.0f, 10.0f);

		spawnParticle(
			position,
			glm::vec3(randX, fountainSpeed + randY, randZ),
			glm::vec4(fountainColor, 1.0f),
			fountainSize,
			fountainLife,
			Particle::Type::FOUNTAIN
		);
	}
}

void Launcher::update(Camera& camera, GLfloat* particle_position, GLubyte* particle_color)
{
	float deltaTime = Camera::getDeltaTime();

	launchTime -= deltaTime;
	if (launchTime <= 0)
	{
		launchFirework();
		launchTime = launchDelay;
	}

	fountainTime -= deltaTime;
	if (fountainTime <= 0) 
	{
		launchFountain();
		fountainTime = fountainDelay;
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

glm::vec3 fadeColor(glm::vec3 startColor, glm::vec3 destColor, float ratio)
{
	return { 
		(ratio * startColor.r + (1.0f - ratio) * destColor.r) / 1.0f,
		(ratio * startColor.g + (1.0f - ratio) * destColor.g) / 1.0f,
		(ratio * startColor.b + (1.0f - ratio) * destColor.b) / 1.0f 
	};
}

float getRandomNumber(float min, float max)
{
	return ((float(rand()) / float(RAND_MAX)) * (max - min)) + min;
}

glm::vec4 getRandomBrightColor()
{
	auto rgb = glm::vec3(0.0f, 0.0f, 0.0f);
	unsigned char region, remainder, p, q, t;

	float h = getRandomNumber(0, 256);
	unsigned char s = 255;
	unsigned char v = 255;

	region = h / 43;
	remainder = (h - (region * 43)) * 6;

	p = (v * (255 - s)) >> 8;
	q = (v * (255 - ((s * remainder) >> 8))) >> 8;
	t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

	switch (region)
	{
	case 0:
		rgb.r = v; rgb.g = t; rgb.b = p;
		break;
	case 1:
		rgb.r = q; rgb.g = v; rgb.b = p;
		break;
	case 2:
		rgb.r = p; rgb.g = v; rgb.b = t;
		break;
	case 3:
		rgb.r = p; rgb.g = q; rgb.b = v;
		break;
	case 4:
		rgb.r = t; rgb.g = p; rgb.b = v;
		break;
	default:
		rgb.r = v; rgb.g = p; rgb.b = q;
		break;
	}

	return glm::vec4(rgb, 1.0f);
}
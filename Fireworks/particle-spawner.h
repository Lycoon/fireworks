#pragma once

#include <glm.hpp>
#include "particle.h"

const int MAX_PARTICLES = 100000;
static GLfloat* g_particule_position_size_data = new GLfloat[MAX_PARTICLES * 4];
static GLubyte* g_particule_color_data = new GLubyte[MAX_PARTICLES * 4];

class ParticleSpawner {
public:
	ParticleSpawner(glm::vec3 position) : position(position) {
		lastParticle = 0;
		particleCount = 0;
	}

	void init();
	void render(double delta);
	void generate_new_particules(double delta);
		
private:
    Particle particles[MAX_PARTICLES];
    int find_unused_particle();

	GLuint billboard_vertex_buffer;
	GLuint particles_position_buffer;
	GLuint particles_color_buffer;
	glm::vec3 position;
	int lastParticle;
	int particleCount;
};
#include "particle-spawner.h"

#include <glad/glad.h>
#include <iostream>
#include <gl/GL.h>

void ParticleSpawner::init() {
	std::cout << "Initializing particle spawner..." << std::endl;

    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
    };

    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
}

int ParticleSpawner::find_unused_particle() {

    for (int i = lastParticle; i < MAX_PARTICLES; i++) {
        if (particles[i].life < 0) {
            lastParticle = i;
            return i;
        }
    }

    for (int i = 0; i < lastParticle; i++) {
        if (particles[i].life < 0) {
            lastParticle = i;
            return i;
        }
    }

    return 0;
}

void ParticleSpawner::generate_new_particules(double delta)
{
    int newParticles = (int)(delta * 10000.0);
    if (newParticles > (int)(0.016f * 10000.0))
        newParticles = (int)(0.016f * 10000.0);

    for (int i = 0; i < newParticles; i++)
    {
        int idx = find_unused_particle();
        particles[idx].life = 5.0f;
        particles[idx].position = glm::vec3(0, 0, -20.0f);

        float spread = 1.5f;
        glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
        glm::vec3 randomdir = glm::vec3(
            (rand() % 2000 - 1000.0f) / 1000.0f,
            (rand() % 2000 - 1000.0f) / 1000.0f,
            (rand() % 2000 - 1000.0f) / 1000.0f
        );

        particles[idx].speed = maindir + randomdir * spread;
        particles[idx].color.r = rand() % 256;
        particles[idx].color.g = rand() % 256;
        particles[idx].color.b = rand() % 256;
        particles[idx].color.a = (rand() % 256) / 3;

        particles[idx].size = (rand() % 1000) / 2000.0f + 0.1f;
    }
}

void ParticleSpawner::render(double delta) 
{
    generate_new_particules(delta);

    particleCount = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) 
    {
        Particle& p = particles[i];
        if (p.life <= 0.0f)
            continue;

        p.life -= delta;
        if (p.life > 0.0f) 
        {
            p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)delta * 0.5f;
            p.position += p.speed * (float)delta;

            g_particule_position_size_data[4 * particleCount + 0] = p.position.x;
            g_particule_position_size_data[4 * particleCount + 1] = p.position.y;
            g_particule_position_size_data[4 * particleCount + 2] = p.position.z;

            g_particule_position_size_data[4 * particleCount + 3] = p.size;

            g_particule_color_data[4 * particleCount + 0] = p.r;
            g_particule_color_data[4 * particleCount + 1] = p.g;
            g_particule_color_data[4 * particleCount + 2] = p.b;
            g_particule_color_data[4 * particleCount + 3] = p.a;
        }

        particleCount++;
    }

    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(GLubyte) * 4, g_particule_color_data);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)0);

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particleCount);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}
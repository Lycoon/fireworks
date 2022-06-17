#pragma once

#include <glm.hpp>
#include <glad/glad.h>
#include <vector>

class VBO
{
public:
	GLuint id;

	VBO(std::vector<GLfloat>& data);

	void bind();
	void unbind();
	void erase();
};
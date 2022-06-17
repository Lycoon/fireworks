#pragma once

#include <glad/glad.h>
#include "VBO.h"

class VAO
{
public:
	GLuint id;

	VAO();

	void linkAttrib(VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
	void bind();
	void unbind();
	void erase();
};
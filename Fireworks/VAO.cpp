#include"VAO.h"

VAO::VAO()
{
	glGenVertexArrays(1, &id);
}

void VAO::linkAttrib(VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	vbo.bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	vbo.unbind();
}

void VAO::bind()
{
	glBindVertexArray(id);
}

void VAO::unbind()
{
	glBindVertexArray(0);
}

void VAO::erase()
{
	glDeleteVertexArrays(1, &id);
}
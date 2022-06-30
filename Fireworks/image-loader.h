#pragma once

#include <glad/glad.h>
#include <fstream>
#include <iostream>

class ImageLoader
{
public:
	ImageLoader();
	GLuint loadBMP_custom(const char* imagepath);

	unsigned int getWidth() const;
	unsigned int getHeight() const;
	unsigned char* getData() const;

private:
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int width, height;
	unsigned int imageSize;
	unsigned char* data;
};

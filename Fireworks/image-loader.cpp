#include "image-loader.h"
#include <fstream>

ImageLoader::ImageLoader() {}

GLuint ImageLoader::loadBMP_custom(const char* imagepath)
{
	FILE* file = fopen(imagepath, "rb");
	if (!file) { printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); return 0; }

	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return 0;
	}

	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	if (imageSize == 0)
		imageSize = width * height * 3;
	if (dataPos == 0)
		dataPos = 54;

	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);

	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	delete[] data;

	return textureId;
}

unsigned int ImageLoader::getWidth() const 
{
	return width;
}

unsigned int ImageLoader::getHeight() const
{
	return height;
}

unsigned char* ImageLoader::getData() const
{
	return data;
}
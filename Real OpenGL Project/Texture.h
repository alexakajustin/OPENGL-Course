#pragma once
#include <GL/glew.h>
#include "stb_image.h"
#include <string.h>
class Texture
{
public:
	Texture();
	Texture(const char* fileLoc);

	void LoadTexture();
	void UseTexture();
	void ClearTexture();

	~Texture();
private:
	// id on graphics card
	GLuint textureID;
	int width, height, bitDepth;
	char* fileLocation;
};


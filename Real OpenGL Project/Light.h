#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Light
{
public:
	~Light();

	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity);

	// Meant to be overriden
	void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation);

private:
	glm::vec3 colour;
	GLfloat ambientIntensity;
};


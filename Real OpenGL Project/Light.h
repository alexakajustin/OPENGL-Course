#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Light
{
public:
	~Light();

	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity, GLfloat xDirection, GLfloat yDirection, GLfloat zDirection, GLfloat diffuseIntensity);

	// Meant to be overriden
	void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation, GLuint diffuseIntensityLocation, GLuint directionLocation);

private:
	// ambient light things
	glm::vec3 colour;
	GLfloat ambientIntensity;

	// diffuse light things
	glm::vec3 direction;
	GLfloat diffuseIntensity;

};


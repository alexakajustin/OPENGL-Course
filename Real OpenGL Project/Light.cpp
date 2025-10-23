#include "Light.h"

Light::~Light()
{
}

Light::Light()
{
	colour = glm::vec3(1.0f, 1.0f, 1.0f);
	ambientIntensity = 1.0f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity)
{
	colour = glm::vec3(red, green, blue);
	this->ambientIntensity = ambientIntensity;
}

void Light::UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation)
{
	glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);
	glUniform1f(ambientIntensityLocation, ambientIntensity);
}

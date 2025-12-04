#pragma once
#include "PointLight.h"
class SpotLight :
    public PointLight
{
public:
    SpotLight();

    SpotLight(GLfloat red, GLfloat green, GLfloat blue,
        GLfloat ambientIntensity, GLfloat diffuseIntensity,
        GLfloat xPosition, GLfloat yPosition, GLfloat zPosition,
        GLfloat xDir, GLfloat yDir, GLfloat zDir,
        GLfloat constant, GLfloat linear, GLfloat exponent,
        GLfloat edge);

    void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
        GLuint diffuseIntensityLocation, GLuint positionLocation, GLuint directionLocation,
        GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation,
        GLuint edgeLocation);
     
    ~SpotLight();
private:
    glm::vec3 direction;

    // angle and processed angle
    GLfloat edge, procEdge;
};


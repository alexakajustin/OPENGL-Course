#include <stdio.h>
#include <string.h>
#include <cmath>
#include <random>
#include <Windows.h>
#include <vector>

#include <GL\glew.h>

#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>


#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
 
const float toRadians = glm::pi<float>() / 180.0f;

Window mainWindow;

std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

GLuint shader;
static const char* vShader = "Shaders/shader.vert";
static const char* fShader = "Shaders/shader.frag";

void CreateObject()
{
	GLuint indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj2);
}

void CreateShader()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(800, 600);

	mainWindow.Initialise();

	CreateObject();

	CreateShader();

	GLuint uniformProjection = 0, uniformModel = 0;

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.03f, 1000.0f);

	srand(time(NULL));

	// ---------------- DONE WITH INITS------------------------------
	// -------------NOW IT IS THE TIME FOR THE LOOP-------------------
	float currentPosition = -1.0f;
	float step = 0.001f;
	float direction = -1.0f;
	while (!mainWindow.getShouldClose())
	{
		// handle user input
		glfwPollEvents();

		// clear window
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();

		currentPosition += step * direction;
		
		if (abs(currentPosition) >= 2.0f)
		{
			direction *= -1;
		}

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(currentPosition, 0.0f, -5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, currentPosition, -5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh();

		glUseProgram(0);

		// swap frame buffers (back -> front)
		mainWindow.swapBuffers();
	}
}

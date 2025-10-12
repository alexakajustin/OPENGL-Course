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


#include "Mesh.h"
#include "Shader.h"
 
// Win dims
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = glm::pi<float>() / 180.0f;

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

#pragma region MAIN
int main()
{
	// initialise GLFW
	if (!glfwInit())
	{
		printf("GLFW INIT FAILED!");
		glfwTerminate();
		return 1;
	}

	// set up GLFW window properties
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// stop backwards compatibility to assure no deprecation
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// forward compatible
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create a window
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Super Game Engine", NULL, NULL);

	if (!mainWindow) {
		printf("GLFW Window Creation failed!");
		glfwTerminate();
		return 1;
	}

	// Get buffer size info(whats on the window is the buffer)
	int bufferWidth, bufferHeight; // done so i can actually access these variables and not just the consts
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// ------- Basically done with GLFW initialisation ------
	// ---------- Now it is time to initialise glew ---------


	// set GLEW context to my only window
	glfwMakeContextCurrent(mainWindow);

	// allow experimental features
	glewExperimental = GL_TRUE;

	// initialise glew
	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialisation failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// set viewport size
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, bufferWidth, bufferHeight);

	CreateObject();

	CreateShader();

	GLuint uniformProjection = 0, uniformModel = 0;

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)bufferWidth / (GLfloat)bufferHeight, 0.03f, 1000.0f);

	srand(time(NULL));

	// ---------------- DONE WITH INITS------------------------------
	// -------------NOW IT IS THE TIME FOR THE LOOP-------------------
	float currentPosition = -1.0f;
	float step = 0.001f;
	float direction = -1.0f;
	while (!glfwWindowShouldClose(mainWindow))
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
		glfwSwapBuffers(mainWindow);
	}

}
#pragma endregion


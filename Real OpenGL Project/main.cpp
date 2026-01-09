#define STB_IMAGE_IMPLEMENTATION

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

#include "CommonValues.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "Material.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "assimp\Importer.hpp"
 
#include "Model.h"


Window mainWindow;

Camera camera;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

GLuint shader;
static const char* vShader = "Shaders/shader.vert";
static const char* fShader = "Shaders/shader.frag";

Texture brickTexture, dirtTexture, plainTexture;

Material shinyMaterial, dullMaterial, plainMaterial;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

Model rug, monitor, statue;

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess,
uniformOmniLightPos, uniformFarPlane;


std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Shader directionalShadowShader;
Shader omniShadowShader;


unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;


void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount, 
						unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);
		
		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	directionalShadowShader = Shader();
	directionalShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
	omniShadowShader.CreateFromFiles("Shaders/omni_shadow_map.vert", "Shaders/omni_shadow_map.geom", "Shaders/omni_shadow_map.frag");
}
float nothing = 0.0f;
float adder = 0.1f;
void RenderScene()
{

	// pyramid 1
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	brickTexture.UseTexture();
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[0]->RenderMesh();

	// pyramid 2
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dirtTexture.UseTexture();
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[1]->RenderMesh();

	// floor
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	plainTexture.UseTexture();
	plainMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	// rug
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	rug.RenderModel();

	// monitor
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 5.0f));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	monitor.RenderModel();
}

void DirectionalShadowMapPass(DirectionalLight* light)
{
	directionalShadowShader.UseShader();

	// make a viewport square viewport 
	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	// activate write mode
	light->GetShadowMap()->Write();
	// clear information
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation();
	directionalShadowShader.SetDirectionalLightTransform(light->CalculateLightTransform());

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmniShadowMapPass(PointLight* light)
{
	omniShadowShader.UseShader();

	// make a viewport square viewport 
	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	// activate write mode
	light->GetShadowMap()->Write();
	// clear information
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = omniShadowShader.GetModelLocation();
	uniformOmniLightPos = omniShadowShader.getOmniLightPosLocation();
	uniformFarPlane = omniShadowShader.getFarPlaneLocation();

	glUniform3f(uniformOmniLightPos, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
	glUniform1f(uniformFarPlane, light->GetFarPlane());
	omniShadowShader.SetLightMatrices(light->CalculateLightTransform());

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
	shaderList[0].UseShader();
	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();

	glViewport(0, 0, 1920, 1080);


	// clear window
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

	shaderList[0].SetDirectionalLight(&mainLight);
	shaderList[0].SetPointLights(pointLights, pointLightCount);
	shaderList[0].SetSpotLights(spotLights, spotLightCount);
	shaderList[0].SetDirectionalLightTransform(mainLight.
		CalculateLightTransform());

	mainLight.GetShadowMap()->Read(GL_TEXTURE1);
	shaderList[0].SetTexture(0);
	shaderList[0].SetDirectionalShadowMap(1);

	glm::vec3 lowerLight = camera.getCameraPosition();
	lowerLight.y -= 0.1f;
	spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

	RenderScene();
}

int main()
{
	mainWindow = Window(1920, 1080);

	mainWindow.Initialise();

	CreateObjects();

	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 1.0f, 0.25f);

	brickTexture = Texture( "Textures/brick.png");
	brickTexture.LoadTextureA();

	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();

	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();

	shinyMaterial = Material(1.0f, 32);
	dullMaterial = Material(0.3f, 4);
	plainMaterial = Material(1.0f, 512);

	rug = Model();
	rug.LoadModel("Models/rug.obj");

	monitor = Model();	
	monitor.LoadModel("Models/monitor.obj");

	statue = Model();
	statue.LoadModel("Models/statue.obj");

	mainLight = DirectionalLight(4096, 4096, // width, height
							     1.0f, 1.0f, 1.0f, // color
								 0.05f, 0.15f, // ambient intensity, diffuse intensity
								 1.0f, -15.0f, -10.0f); // direction
	

	// POINT LIGHTS

	pointLights[0] = PointLight(1024, 1024, // width, height
								0.01f, 100.0f, // near, far
						        0.0f, 1.0f, 0.0f,  // color
								0.5f, 0.05f, // ambient, diffuse
								10.0f, 5.0f, 0.0f, // position
								0.3f, 0.2f, 0.1f); // quadratic eq

	pointLightCount++;

	pointLights[1] = PointLight(1024, 1024, // width, height
		0.01f, 100.0f, // near, far
		0.0f, 0.0f, 1.0f,
		0.25f, 0.5f,
		-10.0f, 5.0f, 0.0f,
		0.3f, 0.2f, 0.1f);

	pointLightCount++;

	// SPOT LIGHTS

	spotLights[0] = SpotLight(1024, 1024, // width, height
		0.01f, 100.0f, // near, far
		1.0f, 1.0f, 1.0f, // red green blue
		1.0f, 1.0f, // ambient, diffuse
		20.0f, 5.0f, 0.0f, // xpos, ypos, zpos
		-50.0f, -1.0f, 0.0f, // xdir, ydir, zdir
		1.0f, 0.1f, 0.1f, // ct lin ex
		20.0f); // edge angle
	spotLightCount++;

	spotLights[1] = SpotLight(1024, 1024, // width, height
							 0.01f, 100.0f, // near, far
							 1.0f, 0.0f, 0.0f, // red green blue
							 1.0f, 1.0f, // ambient, diffuse
							 20.0f, 5.0f, 0.0f, // xpos, ypos, zpos
							 -50.0f, -1.0f, 0.0f, // xdir, ydir, zdir
							 1.0f, 0.1f, 0.1f, // ct lin ex
							 20.0f); // edge angle
	spotLightCount++;

	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.001f, 1000.0f);

	// ---------------- DONE WITH INITS------------------------------
	// -------------NOW IT IS THE TIME FOR THE LOOP-------------------
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		// handle user input
		glfwPollEvents();

		camera.keyControl(mainWindow.getKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		
		DirectionalShadowMapPass(&mainLight);

		for (size_t i = 0; i < pointLightCount; i++)
		{
			OmniShadowMapPass(&pointLights[i]);
		}

		for (size_t i = 0; i < spotLightCount; i++)
		{
			OmniShadowMapPass(&spotLights[i]);
		}


		RenderPass(projection, camera.calculateViewMatrix());

		glUseProgram(0);

		// swap frame buffers (back -> front)
		mainWindow.swapBuffers();
	}
}

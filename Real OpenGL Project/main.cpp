#include <stdio.h>
#include <string.h>
#include <cmath>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
// Win dims
const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, shader, uniformXMove;

bool direction = 0;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.005f;

// vertex shader
static const char* vShader = "			         \n\
#version 330							         \n\
layout (location = 0) in vec3 pos;		         \n\
uniform float xMove;						     \n\
void main()								         \n\
{										   	     \n\
	gl_Position = vec4(pos.x * 0.4 + xMove, pos.y * 0.4, pos.z, 1.0);\n\
}";	


// fragment shader
static const char* fShader = "			         \n\
#version 330							         \n\
out vec4 colour;								 \n\
void main()								         \n\
{										   	     \n\
	colour = vec4(1.0, 0.0, 0.0, 1.0);			 \n\
}";	

void CreateTriangle()
{
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
	};

	glGenVertexArrays(1, &VAO); // how many and where to store the ID
	glBindVertexArray(VAO); // any opengl functions that involve VAOS are now using 

	/// ------------------ USING VAO NOW---------------------------
	glGenBuffers(1, &VBO); // Create ONE buffer for the BINDED VAO
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // use the vbo
	//add the vertices that i have to the vbo
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // static = not changing vertices

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);// stride means if i want color for vertices, pointer means offset from where to start
	glEnableVertexAttribArray(0); // ts just tells the gpu how you lay out data at location 0
	

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind the vbo
	//unbind the vao
	glBindVertexArray(0);
	// -------------------UNBINDED VAO-----------------------------

}

void AddShader(GLuint program, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLenght[1];
	codeLenght[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLenght);

	glCompileShader(theShader);

	GLint result = 0;
	GLchar errorLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		glGetShaderInfoLog(shader, sizeof(errorLog), NULL, errorLog);
		printf("error at shader compile status: %s\n", errorLog);
		return;
	}

	glAttachShader(program, theShader);
}

void CompileShader() 
{
	shader = glCreateProgram();
	if(!shader) 
	{
		printf("error creating shader program!");
		return;
	}

	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar errorLog[1024] = { 0 };

	glLinkProgram(shader); // create the shader exe on the graphics card

	glGetProgramiv(shader, GL_LINK_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(errorLog), NULL, errorLog);
		printf("error at shader linking status: %s\n", errorLog);
		return;

	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(errorLog), NULL, errorLog);
		printf("error at shader validation status: %s\n", errorLog);
		return;
	}

	uniformXMove = glGetUniformLocation(shader, "xMove");

}

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
	glViewport(0, 0, bufferWidth, bufferHeight);
	CreateTriangle();
	CompileShader();

	// ---------------- DONE WITH INITS------------------------------
	// -------------NOW IT IS THE TIME FOR THE LOOP-------------------

	while (!glfwWindowShouldClose(mainWindow)) 
	{
		// handle user input
		glfwPollEvents();
		if (direction) // right 
		{
			triOffset += triIncrement;
		} 
		else
		{
			triOffset -= triIncrement;
		}

		if (abs(triOffset) >= triMaxOffset)
		{
			direction = !direction;
		}

		// clear window
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glUniform1f(uniformXMove, triOffset);

		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, 3);
		
		glBindVertexArray(0);
		
		glUseProgram(0);

		// swap frame buffers (back -> front)
		glfwSwapBuffers(mainWindow);
	}
}
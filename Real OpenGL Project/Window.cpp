#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
}

Window::Window(GLfloat windowWidth, GLfloat windowHeight)
{
	width = windowWidth;
	height = windowHeight;
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}

int Window::Initialise()
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
	mainWindow = glfwCreateWindow(width, height, "Super Game Engine", NULL, NULL);

	if (!mainWindow) {
		printf("GLFW Window Creation failed!");
		glfwTerminate();
		return 1;
	}

	// Get buffer size info(whats on the window is the buffer)
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
}


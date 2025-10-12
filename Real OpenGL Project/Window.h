#pragma once

#include <stdio.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

class Window
{
public:
	Window();
	Window(GLfloat windowWidth, GLfloat windowHeight);
	~Window();

	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }
	void swapBuffers() { glfwSwapBuffers(mainWindow); }
private:
	GLFWwindow* mainWindow;

	GLint width, height;
	GLint bufferWidth, bufferHeight;
};


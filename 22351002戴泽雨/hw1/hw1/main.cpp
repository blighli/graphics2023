#include "Shader.h"
#include "Primative.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <direct.h>

// Called when window size changed.
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;

const unsigned int SCR_HEIGHT = 600;

int main()
{
	// init
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "hw1", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// main
	auto shader = core::shader::Shader("basic_vertex.glsl", "basic_fragment.glsl");
	auto triangle_1 = core::primative::Triangle{
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.5f, 0.0f},
		{1.0f, 1.0f, 0.0f},
		{1.0f, 0.0f, 0.0f, 1.0f},
		{1.0f, 1.0f, 0.0f, 1.0f},
		{1.0f, 0.0f, 1.0f, 1.0f}
	};

	// main loop
	while (!glfwWindowShouldClose(window)) {
	
		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	
		shader.use();
		triangle_1.draw();
	
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "general_algorithm.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void process_input(GLFWwindow* window);

const unsigned int kScreenWidth = 800;
const unsigned int kScreenHeight = 600;

int main()
{
	// init glfw
	// ------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create glfw window
	// -----------------------------
	GLFWwindow* window = glfwCreateWindow(kScreenWidth, kScreenHeight, "triangle", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// load glad
	// ------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to init glad" << std::endl;
		return -1;
	}

	// set opengl viewport
	// ------------------------
	glViewport(0, 0, kScreenWidth, kScreenHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// set triangle vertices data and vertex buffer and configure vertex attributes
	// -----------------------------------

	// triangle vertices data
	float vertices[] = {
		// position          // color
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // right bottom
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // left bottom
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top
	};

	// triangle vertex indices
	int indices[] = {
		0, 1, 2
	};

	// vertex buffer object
	unsigned int VBO;
	// element buffer object
	unsigned int EBO;
	// vertex array object
	unsigned int VAO;
	// generate vertex buffer object 
	glGenBuffers(1, &VBO);
	// generate element buffer object
	glGenBuffers(1, &EBO);
	// generate vertex array object
	glGenVertexArrays(1, &VAO);


	// bind VAO
	glBindVertexArray(VAO);
	// bind VBO to gl array buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// copy triangle vertices data to buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// bind EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// copy triangle indices to buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// vertex position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// vertex color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// init shader using vertex shader source path and fragment shader source path
	Shader shader = Shader("shader/triangle.vs", "shader/triangle.fs");

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		process_input(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// use shader program
		shader.use();

		// draw triangles
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

  // save last window snapshot
  GeneralAlgorithm::Snapshot("snapshot.png", R"(../assets)", kScreenWidth, kScreenHeight, true);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shader.ID);


	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

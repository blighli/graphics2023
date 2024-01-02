#pragma once
//lib
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//std
#include <string>

class Window
{
public:
	Window(int w, int h, std::string name);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	bool shouldClose() { return glfwWindowShouldClose(window); }
	bool wasWindowResized() { return frambufferResized; }
	void resetWindowResizedFlag() { frambufferResized = false; }
	GLFWwindow* getGLFWwindow() const { return window; }
	void processInput();
private:
	static void frambufferResizeCallback(GLFWwindow* window, int width, int height);
	void initWindow();

	int width;
	int height;
	bool frambufferResized = false;

	std::string windowName;
	GLFWwindow* window;
};
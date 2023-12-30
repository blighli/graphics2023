#include "window.h"

//std
#include <stdexcept>

Window::Window(int w, int h, std::string name) : width(w), height(h), windowName(name)
{
	initWindow();
}

Window::~Window()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::processInput()
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void Window::frambufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto lvewindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	lvewindow->frambufferResized = true;
	lvewindow->width = width;
	lvewindow->height = height;
	glViewport(0, 0, width, height);
}

void Window::initWindow()
{
	//初始化
	glfwInit();
	//设置主版本号和此版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//设置profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//创建窗口
	window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	//设置为主要的上下文
	glfwMakeContextCurrent(window);
	//set to be the parent window object
	glfwSetWindowUserPointer(window, this);
	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//重设窗口大小
	glfwSetFramebufferSizeCallback(window, frambufferResizeCallback);

}

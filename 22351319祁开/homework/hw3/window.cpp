#include "window.h"
#include "config.h"
#include"application.h"
static void framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height);
void Window::init(Application* application)
{
	this->application = application;
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	pWindow = glfwCreateWindow(Config::window_width, Config::window_height, Config::app_name.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(pWindow, this->application);
	glfwSetFramebufferSizeCallback(pWindow, framebufferResizeCallback);
	auto mouse_pos_callback = [](GLFWwindow* pWindow, double xpos, double ypos) {
		static float lastX = Config::window_width / 2, lastY = Config::window_height / 2;
		static bool firstMouse = true;

		//右键转动摄像机
		if (glfwGetMouseButton(pWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			auto application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(pWindow));
			if (firstMouse) {
				lastX = xpos;
				lastY = ypos;
				firstMouse = false;
			}

			float offsetX = xpos - lastX;
			float offsetY = -(ypos - lastY); // Y坐标是从上向下增长的，所以取反

			lastX = xpos;
			lastY = ypos;

			application->camera.ProcessMouseMovement(offsetX, offsetY);
		}
		else {
			firstMouse = true; // 当鼠标右键没有按下时重置firstMouse标记
		}
		};
	glfwSetCursorPosCallback(pWindow, mouse_pos_callback);
	auto mouse_click_callback = [](GLFWwindow* window, int button, int action, int mods) {
		
		};
	glfwSetMouseButtonCallback(pWindow, mouse_click_callback);
	auto scroll_callback = [](GLFWwindow* pWindow, double xoffset, double yoffset)
		{
			auto application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(pWindow));
			application->camera.ProcessMouseScroll(static_cast<float>(yoffset));
		};
	glfwSetScrollCallback(pWindow, scroll_callback);
}
void Window::process_key_input(const float deltaTime) {
	if (glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(pWindow, true);
	}
	using enum Camera_Movement;
	if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS)
		application->camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS)
		application->camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS)
		application->camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS)
		application->camera.ProcessKeyboard(RIGHT, deltaTime);
};

static void framebufferResizeCallback(GLFWwindow* pWindow, int width, int height) {
	auto application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(pWindow));
	application->engine.framebufferResized=true ;
}
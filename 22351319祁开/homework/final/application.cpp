#include "application.h"

Application::Application():resource(engine)
{
	currunt = this;
}

Application::~Application()
{

}

Application* Application::currunt=nullptr;

void Application::init()
{
	window.init(this);
	engine.init(this);
	ui.init(this);
	resource.init();
	camera = Camera(glm::vec3(0., 0., 3.), glm::vec3(0., 1., 0.));
}
void Application::mainLoop() {
	while (!glfwWindowShouldClose(window.pWindow)) {
		glfwPollEvents();
		float currentFrame = static_cast<float>(glfwGetTime());
		static float lastFrame = currentFrame; // 上一帧的时间
		static float deltaTime;
		deltaTime = currentFrame- lastFrame;
		lastFrame = currentFrame;
		/*auto pressInput = [this](GLFWwindow* window) {
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetWindowShouldClose(window, true);
			}
			using enum Camera_Movement;
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				camera.ProcessKeyboard(FORWARD, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				camera.ProcessKeyboard(BACKWARD, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				camera.ProcessKeyboard(LEFT, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				camera.ProcessKeyboard(RIGHT, deltaTime);
			};
		pressInput(window);*/
		window.process_key_input(deltaTime);
		ui.show();
		resource.update();
		engine.draw(deltaTime);
	}
	vkDeviceWaitIdle(engine.device);
}
void Application::run()
{
	init();
	mainLoop();
}


#pragma once

#include "window.h"
#include "camera.h"
#include "shader.h"
#include "sphere.h"

class App
{
public:
	static constexpr int WIDTH = 1920;
	static constexpr int HEIGHT = 1080;
	float deltaTime = 0.0f;	
	float lastFrame = 0.0f;

	App();
	~App();

	App(const App&) = delete;
	App& operator=(const App&) = delete;
	void run();
	void processKeyInput(GLFWwindow* window);
private:
	Window window{ WIDTH, HEIGHT, "OpengGL!" };
};
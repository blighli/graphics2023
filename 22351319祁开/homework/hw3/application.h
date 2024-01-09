#pragma once
#include "engine.h"
#include "window.h"
#include "UI.h"
#include "utils.h"
#include "camera.h"
class Application {
public:
	Application();
	~Application();
public:
	static Application *currunt;
	Window window;
	Engine engine;
	UI ui;
	Camera camera;
	void init();
	void mainLoop();
	void run();
private:
};
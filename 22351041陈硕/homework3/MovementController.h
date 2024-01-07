#pragma once

#include "window.h"
#include "camera.h"

#include <glm/glm.hpp>
#include <iostream>

class MovementController
{
public:
	void ProcessMouseMovement(GLFWwindow* window, float dt, Camera& camera);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void initMouse(GLFWwindow* window);
	void moveInPlaneXZ(GLFWwindow* window, float dt, Camera& camera);

	static bool state;
};
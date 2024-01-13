#include<iostream>
#include<GLAD/glad.h>
#include<GLFW/glfw3.h>

#include"texture.h"
#include"resource_manager.h"
#include"shader.h"
#include"sprite_renderer.h"
#include"Game.h"

#include<glm/glm.hpp>
#include<glm/gtx/transform.hpp>

//---test---

//---test---

unsigned int W_WIDTH = 1080, W_HEIGHT = 720;
unsigned int WIDTH = 450, HEIGHT = 720;
float deltaTime = 0.f, lastFrame = 0.f, currentFrame = 0.f;
GLFWwindow* window=NULL;
bool Init();
void key_callback(GLFWwindow*,int,int,int,int);




int main()
{
	if (!Init())
		return -1;
	glfwSetKeyCallback(window, key_callback);
	glViewport(0, 0, W_WIDTH, W_HEIGHT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	MyGame::Game& game = MyGame::getGame();
	game.Init(WIDTH, HEIGHT,W_WIDTH,W_HEIGHT);
	

	lastFrame= glfwGetTime();


	while (!glfwWindowShouldClose(window)) {
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwPollEvents();
	
		//render
		//std::cout << deltaTime << std::endl;
		if (deltaTime > 0.01f) {
			deltaTime = 0.01f;
			//deltaTime = 0.01f;
			game.update(deltaTime);
			game.ProcessInput(deltaTime);
			game.render();
			lastFrame = currentFrame;


			glfwSwapBuffers(window);
		}
	}
	return 0;
}

void key_callback(GLFWwindow*window,int key,int scancode, int action ,int mode) {
	MyGame::Game& game = MyGame::getGame();
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS)
			game.setKeys(key, true);
		else if (action == GLFW_RELEASE)
			game.setKeys(key, false);
	}
}

bool Init() {
	if (!glfwInit())
		return false;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "MyGame", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	
	return true;
}
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightPos2(5.0f, 1.0f, 2.0f);
glm::vec3 lightPos3(5.0f, 1.0f, 2.0f);
glm::vec3 lightPos4(5.0f, 1.0f, 2.0f);
float scale = 0.5f;
static const float pai = 3.1415926f;
float objLocX = 0.f, objLocY = 0.f, objLocZ = 0.f;
static GLfloat _rotate_z = 0.0f;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

float toRadians(float degrees)
{
	return (degrees * 2.f * pai) / 360.f;
}

int main()
{
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ModelDemo", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "创建GLFW窗体失败" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);


	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "初始化GLAD失败" << std::endl;
		return -1;
	}

	
	glEnable(GL_DEPTH_TEST);

	
	Shader ourShader("vertexShader.vert", "fragShader.frag");
	Model ourModel("res/bicycle_L3.123c72dde4e2-4bc7-4bb2-84b3-77a00667d146/10489_bicycle_L2.obj");


	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		lightPos.x = cos(currentFrame) * 2.0;
		lightPos.y = sin(currentFrame) * 2.0;
		lightPos2.x = cos(currentFrame) * 2.0;
		lightPos2.z = sin(currentFrame) * 2.0;

		lightPos3.x = sin(currentFrame/3+200) * 2.0;
		lightPos3.z = cos(currentFrame/3+100) * 2.0;
		lightPos4.x = sin(currentFrame/8+50) * 2.0;
		lightPos4.z = cos(currentFrame/8+10) * 2.0;
		processInput(window);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ourShader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);
		ourShader.set3f("lightColor", 1.0f, 0.0f, 0.0f);
		ourShader.set3f("lightPos", lightPos.x, lightPos.y, lightPos.z);

		ourShader.set3f("lightColor2", 0.0f, 1.0f, 0.0f);
		ourShader.set3f("lightPos2", lightPos2.x, lightPos2.y, lightPos2.z);

		ourShader.set3f("lightColor3", 0.0f, 0.0f, 1.0f);
		ourShader.set3f("lightPos3", lightPos3.x, lightPos3.y, lightPos3.z);

		ourShader.set3f("lightColor4", 1.0f, 1.0f, 1.0f);
		ourShader.set3f("lightPos4", lightPos4.x, lightPos4.y, lightPos4.z);
		
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.005, 0.005, 0.005));
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -1.0f)); 
		model = glm::rotate(model, toRadians(-90), glm::vec3(1.f, 0.f, 0.f));

		model = glm::scale(model, glm::vec3(scale, scale, scale));
		model = glm::translate(model, glm::vec3(objLocX, objLocY, objLocZ));
		model = glm::rotate(model, toRadians(_rotate_z), glm::vec3(0.f, 0.f, 1.f));
		ourShader.setMat4("model", model);
		ourModel.Draw(ourShader);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera::RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Z)== GLFW_PRESS)
		_rotate_z = _rotate_z + 0.01f;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		objLocX = objLocX + 0.001f;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		objLocY = objLocY + 0.001f;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		objLocZ = objLocZ + 0.001f;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		scale = scale + 0.0001f;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		scale = scale - 0.0001f;
	
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; 
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


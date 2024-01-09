#include <bits/stdc++.h>
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "utils.hpp"
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

using namespace std;
using namespace glm;

glm::vec2 mousePos;

const unsigned int SCR_WIDTH = 512;
const unsigned int SCR_HEIGHT = 512;

Camera cam = Camera(glm::vec3(0, 1.571, 10), glm::vec3(0.0, 0.0, 0.0), 1);
bool mousePressed = false;
glm::vec3 cubePositions[] = {
    glm::vec3(1.3f,  4.2f,  1.5f),
    glm::vec3(-1.3f, 4.3f,  -1.5f)
};

std::vector<struct cubeLight> cubeLight;
struct Cube aCube;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_W) {
		cam.moveCamera(0.0, -1.0, 0.0);
	}

	if (key == GLFW_KEY_S) {
		cam.moveCamera(0.0, 1.0, 0.0);
	}

	if (key == GLFW_KEY_A) {
		cam.moveCamera(1.0, 0.0, 0.0);
	}

	if (key == GLFW_KEY_D) {
		cam.moveCamera(-1.0, 0.0, 0.0);
	}
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if ((action == GLFW_PRESS) || (action == GLFW_RELEASE))
		mousePressed = !mousePressed;
}

void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	glm::vec2 newPos = glm::vec2(xpos / (double)vp[2], -ypos / (double)vp[3]) * 2.f - glm::vec2(1.f);

	glm::vec2 diff = newPos - mousePos;
	if (mousePressed)
		cam.translateCamera(-diff.y, diff.x, 0.0);

	mousePos = newPos;
}

void resizeCallback(GLFWwindow* window, int width, int height)
{
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	float aspectRatio = (float)vp[2] / (float)vp[3];

	if (width > height) {
		int newWidth = height * aspectRatio;
		glViewport((width - newWidth) / 2, 0, newWidth, height);
	}
	else {
		int newHeight = width / aspectRatio;
		glViewport(0, (height - newHeight) / 2, width, newHeight);
	}
}


void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
	cam.translateCamera(0.0, 0.0, -yOffset);
}

void getCube() {
    //���������������Ͷ��㻺�����?

    glGenVertexArrays(1, &aCube.VAO);
    glGenBuffers(1, &aCube.VBO);
    glGenBuffers(1, &aCube.EBO);

    //�����ݰ󶨵����㻺������
    glBindVertexArray(aCube.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, aCube.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aCube.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //���ö�������ָ��
    //����location 0 ,3��ֵ�����ͣ���׼���񣬴�С��ƫ����
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    //############## ����׼��
    glGenTextures(1, &aCube.texture1);
    glBindTexture(GL_TEXTURE_2D, aCube.texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    //stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("world/world_giant.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture1" << std::endl;
    }
    stbi_image_free(data);
}

void addCubeLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse,
    glm::vec3 specular, float constant, float linear, float quadratic, glm::vec3 color) {
    cubeLight.push_back({ position, ambient, diffuse, specular, constant, linear, quadratic,color });
}
int main()
{
    // ��ʼ������
    glfwInit();
// glfwSetErrorCallback(glfw_error_callback);
	// assert(glGetError() == GL_NO_ERROR && "Failed to init GLFW");
    
	GLFWwindow* window = glfwCreateWindow(1024, 1024, "Model load 22351146", 0, 0);
	// assert(window && "Failed to create window");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PENG 22051155 project03 3DModel", NULL, NULL);
    // if (window == NULL)
    // {
    //     std::cout << "Failed to create GLFW window" << std::endl;
    //     glfwTerminate();
    //     return -1;
    // }
    glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetScrollCallback(window, mouseScrollCallback);
	glfwSetWindowSizeCallback(window, resizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);
    Shader ourShader("model_vs.glsl", "model_fs.glsl");
    Shader lightCubeShader("shader_vs.glsl", "shader_fs.glsl");
    Model ourModel(("world/world.obj"));
    getCube();

    addCubeLight(glm::vec3(0.f, 0.f, 10.f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.4f, 0.4f),
        glm::vec3(0.0f, 1.0f, 1.0f),
        1.0f, 0.09f, 0.032, glm::vec3(0.4f, 1.0f, 0.4f));

    addCubeLight(glm::vec3(0.f, 5.f, -5.f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.4f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        1.0f, 0.09f, 0.032, glm::vec3(1.0f, 0.4f, 0.4f));

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(120.f), 1.f, 0.1f, 100.0f);
        glm::mat4 view = cam.getMatrix();
        ourShader.setMat4("perspectiveMatrix", projection);
        ourShader.setMat4("cameraMatrix", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);


        ourShader.use();
        ourShader.setVec3("viewPos", cam.dir);
        ourShader.setInt("pointLightNum", 2);
        ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);


        ourShader.setVec3("pointLights[0].position", cubeLight[0].position);
        ourShader.setVec3("pointLights[0].ambient", cubeLight[0].ambient);
        ourShader.setVec3("pointLights[0].diffuse", cubeLight[0].diffuse);
        ourShader.setVec3("pointLights[0].specular", cubeLight[0].specular);
        ourShader.setFloat("pointLights[0].constant", cubeLight[0].constant);
        ourShader.setFloat("pointLights[0].linear", cubeLight[0].linear);
        ourShader.setFloat("pointLights[0].quadratic", cubeLight[0].quadratic);

        ourShader.setVec3("pointLights[1].position", cubeLight[1].position);
        ourShader.setVec3("pointLights[1].ambient", cubeLight[1].ambient);
        ourShader.setVec3("pointLights[1].diffuse", cubeLight[1].diffuse);
        ourShader.setVec3("pointLights[1].specular", cubeLight[1].specular);
        ourShader.setFloat("pointLights[1].constant", cubeLight[1].constant);
        ourShader.setFloat("pointLights[1].linear", cubeLight[1].linear);
        ourShader.setFloat("pointLights[1].quadratic", cubeLight[1].quadratic);

        ourShader.setVec3("spotLight.position", cam.pos);
        ourShader.setVec3("spotLight.direction", cam.dir);
        ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("spotLight.constant", 1.0f);
        ourShader.setFloat("spotLight.linear", 0.09);
        ourShader.setFloat("spotLight.quadratic", 0.032);
        ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        ourModel.Draw(ourShader);

        lightCubeShader.use();
        lightCubeShader.setMat4("perspectiveMatrix", projection);
        lightCubeShader.setMat4("cameraMatrix", view);

        glBindVertexArray(aCube.VAO);
        for (unsigned int i = 0; i < 2; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::scale(model, glm::vec3(0.1f));
            lightCubeShader.setMat4("model", model);
            lightCubeShader.setVec3("lightColor", cubeLight[i].color);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &aCube.VAO);
    glDeleteBuffers(1, &aCube.VBO);

    glfwTerminate();
    return 0;
}

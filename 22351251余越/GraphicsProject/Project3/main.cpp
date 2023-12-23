#include <iostream>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "Config.h"

#include "Shader.h"
#include "Model.h"
#include "Camera.h"
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// 相机定义
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// 计时
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    //test();
    // 初始化GLFW
    glfwInit();
    // 指定GLFW版本号和核心模式
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建GLFW绘制主窗口
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GLFW Window", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 相机交互事件绑定
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // 加载GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    Model ourModel("res/Station/up_base.obj");

    Shader ourShader("shader.vts", "shader.fgs");
    ourShader.use();

    glEnable(GL_DEPTH_TEST);

    // render 循环
    while (!glfwWindowShouldClose(window))
    {
        // 计算每帧经过时间
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //cout << deltaTime << endl;

        //响应输入
        processInput(window);

        glClearColor(0.5, 0.5, 0.5, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        ourShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setInt("lightNum", 3);
        ourShader.setVec3("lights[0].position", 0, 0, 0);
        ourShader.setVec3("lights[0].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("lights[0].diffuse", 0.0f, 0.0f, 0.8f);
        ourShader.setVec3("lights[0].specular", 0.0f, 0.0f, 1.0f);

        ourShader.setVec3("lights[1].position", 0, 5, 0);
        ourShader.setVec3("lights[1].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("lights[1].diffuse", 0.8f, 0.8f, 0.0f);
        ourShader.setVec3("lights[1].specular", 1.0f, 1.0f, 0.0f);

        ourShader.setVec3("lights[2].position", 0, 0, -5);
        ourShader.setVec3("lights[2].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("lights[2].diffuse", 0.0f, 0.8f, 0.0f);
        ourShader.setVec3("lights[2].specular", 0.0f, 1.0f, 0.0f);

        // 投影变换
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, NEAR_DISTANCE, FAR_DISTANCE);
        ourShader.setMat4("projection", projection);

        // View变换
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        ourShader.setMat4("model", model);

        ourModel.Draw(ourShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 退出程序
    glfwTerminate();
    return 0;
}

bool isPressing = false;
// 输入处理
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !isPressing)
    {
        isPressing = true;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE && isPressing)
    {
        camera.mode *= -1;
        isPressing = false;
    }
}

// 自适应窗口大小
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 鼠标指针输入处理
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

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

// 鼠标滚轮输入处理
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset), deltaTime);
}

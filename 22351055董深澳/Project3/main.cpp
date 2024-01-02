#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

#include "shader.h"
#include "camera.h"
#include "model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


float mixValue = 0.2f;

int screenWidth = 800;
int screenHeight = 600;

//摄像机变量
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

//鼠标上一帧的位置
float lastX = 400, lastY = 300;

bool firstMouse = true;    //是否第一次获取鼠标输入

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetScrollCallback(window, scroll_callback);

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    Shader ourShader("model_loading.vs", "model_loading.fs");
    ourShader.use();

    Model ourModel("nanosuit/nanosuit.obj");

    //点光源位置
    glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f,  0.2f,  2.0f),
    glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3(0.0f,  0.0f, -3.0f)
    };


    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        // 输入
        processInput(window);


        // 渲染指令
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view;

        view = camera.GetViewMatrix();

        //投影矩阵
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

        //物体立方体
        ourShader.use();

        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);

        //定向光
        ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);

        ourShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        //点光源
        // point light 1
        ourShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        ourShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[0].constant", 1.0f);
        ourShader.setFloat("pointLights[0].linear", 0.09f);
        ourShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        ourShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        ourShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[1].constant", 1.0f);
        ourShader.setFloat("pointLights[1].linear", 0.09f);
        ourShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        ourShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        ourShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[2].constant", 1.0f);
        ourShader.setFloat("pointLights[2].linear", 0.09f);
        ourShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        ourShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        ourShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[3].constant", 1.0f);
        ourShader.setFloat("pointLights[3].linear", 0.09f);
        ourShader.setFloat("pointLights[3].quadratic", 0.032f);

        //聚光
        ourShader.setVec3("spotLight.position", camera.Position);
        ourShader.setVec3("spotLight.direction", camera.Front);
        ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);

        ourShader.setVec3("spotLight.position", camera.Position);
        ourShader.setVec3("spotLight.direction", camera.Front);
        ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

        ourShader.setFloat("spotLight.constant", 1.0f);
        ourShader.setFloat("spotLight.linear", 0.09f);
        ourShader.setFloat("spotLight.quadratic", 0.032f);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        // 检查并调用事件，交换缓冲
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mixValue += 0.001f;
        if (mixValue > 1.0f)
            mixValue = 1.0f;
    }


    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        mixValue -= 0.001f;
        if (mixValue < 0.0f)
            mixValue = 0.0f;
    }

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) // 这个bool变量初始时是设定为true的
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


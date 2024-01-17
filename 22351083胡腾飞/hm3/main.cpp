#include <windows.h>
#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include<stdio.h>
#include<iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#define M_PI 3.1415926
#include"camara.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include<cstring>
#include<vector>
#include "Mesh.h"
#include "Model.h"
//函数声明
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void mouse_clickcallback(GLFWwindow* window, int button, int action, int mods);
// 设置
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
bool Pattern = true;
glm::mat4 model = glm::mat4(1.0f);
int axischoose = 0;
// 摄像头
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


int main()
{
    //模型初位置设置
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
    /*-------------------初始化--------------------*/
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Use the H key to switch browsing mode, roaming mode/Model translation and rotation mode", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_clickcallback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    /*---------------------------------------------*/
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("", aiProcess_Triangulate | aiProcess_FlipUVs);
    //着色器定义
    Shader lightingShader("shader.vs", "shader.fs");
    glfwSetWindowUserPointer(window, &lightingShader);
    Shader lightCubeShader("lightcode.vs", "lightcode.fs");

    //正方形顶点和cube位置
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    // 光源位置
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.0f,  1.0f,  1.0f),
        glm::vec3(-0.5f, 0.5f, 0.0f),
        glm::vec3(0.5f,  0.5f, 0.0f),
        glm::vec3(0.0f,  0.5f, -1.0f)
    };

    Model ourModel("../model/nanosuit.obj");
    /*-----------------顶点数组对象--------------------*/
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    /*--------------------------------------------------*/
    // 着色器设置
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);

    //循环渲染
    while (!glfwWindowShouldClose(window))
    {
        //时间
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //按键
        processInput(window);

        //刷新底色
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //物品着色器启用
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);

        /*---------------------------参数设置-------------------------------------------*/
        //平行光
        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        //点光源1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.09f);
        lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
        //点光源2
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", 0.09f);
        lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
        //点光源3
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09f);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
        //点光源4
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[3].constant", 1.0f);
        lightingShader.setFloat("pointLights[3].linear", 0.09f);
        lightingShader.setFloat("pointLights[3].quadratic", 0.032f);

        //观察与投影
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setM4fv("projection", projection);
        lightingShader.setM4fv("view", view);

        //世界系矩阵
        lightingShader.setM4fv("model", model);
        lightingShader.setM4fv("model", model);
        ourModel.Draw(lightingShader);
        //光源着色器启用
        lightCubeShader.use();
        lightCubeShader.setM4fv("projection", projection);
        lightCubeShader.setM4fv("view", view);

        //光源绘制
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            glm::mat4 modellight;
            modellight = glm::mat4(1.0f);
            modellight = glm::translate(modellight, pointLightPositions[i]);
            modellight = glm::scale(modellight, glm::vec3(0.02f)); // Make it a smaller cube
            lightCubeShader.setM4fv("model", modellight);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //解绑
    //glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    //结束
    glfwTerminate();
    return 0;
}

//按键设置
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (Pattern) {

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
            Pattern = false;
            Sleep(200);
        }
    }
    else {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            model = glm::translate(model, glm::vec3(0.0f, 0.001f, 0.0f));
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            model = glm::translate(model, glm::vec3(0.0f, -0.001f, 0.0f));
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            model = glm::translate(model, glm::vec3(-0.001f, 0.0f, 0.0f));
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            model = glm::translate(model, glm::vec3(0.001f, 0.0f, 0.0f));
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.001f));
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.001f));
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            axischoose = (axischoose + 1) % 3;
            Sleep(200);
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            axischoose = (axischoose + 2) % 3;
            Sleep(200);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            if (axischoose == 0)
                model = glm::rotate(model, 0.001f, glm::vec3(1.0f, 0.0f, 0.0f));
            if (axischoose == 1)
                model = glm::rotate(model, 0.001f, glm::vec3(0.0f, 1.0f, 0.0f));
            if (axischoose == 2)
                model = glm::rotate(model, 0.001f, glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            if (axischoose == 0)
                model = glm::rotate(model, -0.001f, glm::vec3(1.0f, 0.0f, 0.0f));
            if (axischoose == 1)
                model = glm::rotate(model, -0.001f, glm::vec3(0.0f, 1.0f, 0.0f));
            if (axischoose == 2)
                model = glm::rotate(model, -0.001f, glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
            Pattern = true;
            Sleep(200);
        }
    }
}

//屏幕被改变大小时调用
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

//鼠标移动
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
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    if (Pattern) {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

//滚轮
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (Pattern) {
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
    else {
        if (yoffset > 0)
            model = glm::scale(model, glm::vec3(0.9f, 0.9f, 0.9f));
        else
            model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
    }
}
void mouse_clickcallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        ;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        ;
}


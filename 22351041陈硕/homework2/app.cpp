#include "app.h"
//libs
#define STB_IMAGE_IMPLEMENTATION
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "import/stb_image.h"
//std
#include <stdexcept>
#include <array>
#include <chrono>
#include <iostream>

Camera camera;
Sphere earth;
Sphere moon;
Sphere sun(20, 20, glm::vec3(0.0, 0.0, 15.0), 1.5);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_presscallback(GLFWwindow* window, int button, int action, int mods);
uint32_t loadTexture(char const* path);
float getdist(Sphere& sphere);
bool getdir(Sphere& sphere);
const GLfloat PI = 3.14159f;
const float MAX_FRAME_TIME = 1;
bool firstMouse = true;
float earthdist = 0;
bool earthdir = false;
float moondist = 0;
bool moondir = false;
float sundist = 0;
bool sundir = false;
float lastX;
float lastY;

App::App(){}

App::~App() {}

void App::run()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    //深度测试
    glEnable(GL_DEPTH_TEST);

    //一些事前属性
    lastX = WIDTH / 2.0f;
    lastY = HEIGHT / 2.0f;
    glfwSetCursorPosCallback(window.getGLFWwindow(), mouse_callback);
    glfwSetMouseButtonCallback(window.getGLFWwindow(), mouse_presscallback);
    glfwSetScrollCallback(window.getGLFWwindow(), scroll_callback);
    camera.Position = glm::vec3(0.0f, 0.0f, 3.0f);
    glViewport(0, 0, WIDTH, HEIGHT);
    //着色器
    Shader lightshader{"light.vert", "light.frag"};
    Shader shader{ "vertshader.vert", "fragshader.frag" };
    Shader mouse{ "mouse.vert", "mouse.frag" };
    // 设置顶点数据
    std::vector<float> earthVertices = earth.getVertices();
    std::vector<int> earthIndices = earth.getIndices();
    std::vector<float> earthNormals = earth.getNormals();
    std::vector<float> earthTexcoords = earth.getTexCoords();
    const int eIndices = earth.X_SEGMENTS * earth.Y_SEGMENTS * 6;
    const int elength = eIndices * 5;
    float earthvertices[19200];
    for (int i = 0; i < eIndices; i++) {		
        int index = earthIndices[i];
        earthvertices[i * 8 + 0] = earthVertices[index * 3 + 0];
        earthvertices[i * 8 + 1] = earthVertices[index * 3 + 1];
        earthvertices[i * 8 + 2] = earthVertices[index * 3 + 2];
        earthvertices[i * 8 + 3] = earthNormals[index * 3 + 0];
        earthvertices[i * 8 + 4] = earthNormals[index * 3 + 1];
        earthvertices[i * 8 + 5] = earthNormals[index * 3 + 2];
        earthvertices[i * 8 + 6] = earthTexcoords[index * 2 + 0];
        earthvertices[i * 8 + 7] = earthTexcoords[index * 2 + 1];
    }
    std::vector<float> moonVertices = moon.getVertices();
    std::vector<int> moonIndices = moon.getIndices();
    std::vector<float> moonNormals = moon.getNormals();
    std::vector<float> moonTexcoords = moon.getTexCoords();
    const int mIndices = moon.X_SEGMENTS * moon.Y_SEGMENTS * 6;
    const int mlength = mIndices * 5;
    float moonvertices[19200];
    for (int i = 0; i < mIndices; i++) {		
        int index = moonIndices[i];
        moonvertices[i * 8 + 0] = moonVertices[index * 3 + 0];
        moonvertices[i * 8 + 1] = moonVertices[index * 3 + 1];
        moonvertices[i * 8 + 2] = moonVertices[index * 3 + 2];
        moonvertices[i * 8 + 3] = moonNormals[index * 3 + 0];
        moonvertices[i * 8 + 4] = moonNormals[index * 3 + 1];
        moonvertices[i * 8 + 5] = moonNormals[index * 3 + 2];
        moonvertices[i * 8 + 6] = moonTexcoords[index * 2 + 0];
        moonvertices[i * 8 + 7] = moonTexcoords[index * 2 + 1];
    }
    glm::vec3 modelPositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(1.5f,  1.5f, 0.0f)
    };

    glm::vec3 lightPos = sun.core;

    std::vector<float> sunVertices = sun.getVertices();
    std::vector<int> sunIndices = sun.getIndices();
    std::vector<float> sunNormals = sun.getNormals();
    std::vector<float> sunTexcoords = sun.getTexCoords();
    const int sunnumIndices = sun.X_SEGMENTS * sun.Y_SEGMENTS * 6;
    float sunvertices[12000];
    for (int i = 0; i < sunnumIndices; i++) {		
        int index = sunIndices[i];
        sunvertices[i * 5 + 0] = sunVertices[index * 3 + 0];
        sunvertices[i * 5 + 1] = sunVertices[index * 3 + 1];
        sunvertices[i * 5 + 2] = sunVertices[index * 3 + 2];
        sunvertices[i * 5 + 3] = sunTexcoords[index * 2 + 0];
        sunvertices[i * 5 + 4] = sunTexcoords[index * 2 + 1];
    }
    float mouseP[18] = {
        0.01f,0.0f,0.0f,      1.0f,1.0f,1.0f,
        0.0f,0.01f,0.0f,      1.0f,1.0f,1.0f,
        0.0f,-0.005f,0.0f,      1.0f,1.0f,1.0f,
    };
    // 创建一个顶点数组对象VAO和一个顶点缓冲对象VBO
    uint32_t VBO, earthVAO;
    glGenVertexArrays(1, &earthVAO);
    glGenBuffers(1, &VBO);
    // 绑定VAO，绑定VBO到GL_ARRAY_BUFFER目标上，绑定EBO
    glBindVertexArray(earthVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 将顶点数组复制到缓冲中供OpenGL使用
    glBufferData(GL_ARRAY_BUFFER, sizeof(earthvertices), earthvertices, GL_STATIC_DRAW);
    // 设置顶点属性指针
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 法向量属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //纹理属性
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // 创建一个顶点数组对象VAO
    uint32_t VBOm, moonVAO;
    glGenVertexArrays(1, &moonVAO);
    glGenBuffers(1, &VBOm);
    // 绑定VAO，绑定VBO到GL_ARRAY_BUFFER目标上，绑定EBO
    glBindVertexArray(moonVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBOm);
    // 将顶点数组复制到缓冲中供OpenGL使用
    glBufferData(GL_ARRAY_BUFFER, sizeof(moonvertices), moonvertices, GL_STATIC_DRAW);
    // 设置顶点属性指针
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 法向量属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //纹理属性
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //光源
    // 创建一个顶点数组对象VAO和一个顶点缓冲对象VBO
    uint32_t VBO1, sunVAO;
    glGenVertexArrays(1, &sunVAO);
    glGenBuffers(1, &VBO1);
    // 绑定VAO，绑定VBO到GL_ARRAY_BUFFER目标上，绑定EBO
    glBindVertexArray(sunVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    // 将顶点数组复制到缓冲中供OpenGL使用
    glBufferData(GL_ARRAY_BUFFER, sizeof(sunvertices), sunvertices, GL_STATIC_DRAW);
    // 设置顶点属性指针
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //纹理属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //鼠标
    // 创建一个顶点数组对象VAO和一个顶点缓冲对象VBO
    uint32_t VBO2, mouseVAO;
    glGenVertexArrays(1, &mouseVAO);
    glGenBuffers(1, &VBO2);
    // 绑定VAO，绑定VBO到GL_ARRAY_BUFFER目标上，绑定EBO
    glBindVertexArray(mouseVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    // 将顶点数组复制到缓冲中供OpenGL使用
    glBufferData(GL_ARRAY_BUFFER, sizeof(mouseP), mouseP, GL_STATIC_DRAW);
    // 设置顶点属性指针
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //纹理
    uint32_t texture1 = loadTexture("resources/textures/earth.jpg");
    uint32_t texture2 = loadTexture("resources/textures/moon.jpg"); 
    uint32_t suntexture = loadTexture("resources/textures/sun.jpg");
    
    //着色器设置
    shader.use();
    shader.setInt("material.diffuse", 0);

    lightshader.use();
    lightshader.setInt("Texture", 0);

    //渲染循环
    while (!window.shouldClose())
    {
        //时间
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 方向
        earthdist = getdist(earth);
        earthdir = getdir(earth);
        moondist = getdist(moon);
        moondir = getdir(moon);
        sundist = getdist(sun);
        sundir = getdir(sun);

        //检查按键是否按下
        window.processInput();
        processKeyInput(window.getGLFWwindow());

		//渲染指令
		//glClearColor设置清空屏幕所用的颜色----状态设置函数
		//glClear函数清除颜色缓冲之后，屏幕会填充为glClearColor中的颜色。---状态使用函数
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //激活着色器
        shader.use();
        shader.setVec3("viewPos", camera.Position);
        shader.setFloat("material.shininess", 32.0f);
        shader.setVec3("material.specular", 0.5, 0.5, 0.5);

        // point light
        shader.setVec3("pointLights.position", lightPos);
        shader.setVec3("pointLights.ambient", 0.2f, 0.2f, 0.2f);
        shader.setVec3("pointLights.diffuse", 0.5f, 0.5f, 0.5f);
        shader.setVec3("pointLights.specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights.constant", 1.0f);
        shader.setFloat("pointLights.linear", 0.09f);
        shader.setFloat("pointLights.quadratic", 0.032f);

        //转换矩阵
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

        //设置转换
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);

        //纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        //draw
        glBindVertexArray(earthVAO);
        glm::mat4 model1 = glm::mat4(1.0f);
        model1 = glm::translate(model, modelPositions[0]);
        shader.setMat4("model", model1);
        glDrawArrays(GL_TRIANGLES, 0, eIndices);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glBindVertexArray(moonVAO);
        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, modelPositions[1]);
        model2 = glm::scale(model2, glm::vec3(0.25f));
        shader.setMat4("model", model2);
        glDrawArrays(GL_TRIANGLES, 0, mIndices);

        //光源
        lightshader.use();
        lightshader.setMat4("projection", projection);
        lightshader.setMat4("view", view);
        lightshader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, suntexture);
        glBindVertexArray(sunVAO);
        glDrawArrays(GL_TRIANGLES, 0, sunnumIndices);

        //鼠标
        mouse.use();
        mouse.setMat4("projection", projection);
        mouse.setMat4("view", view);
        mouse.setMat4("model", model);
        mouse.setVec3("up", camera.Up);
        mouse.setVec3("cross", glm::normalize(glm::cross(camera.Front, camera.Up)));
        mouse.setVec3("center", camera.Position + camera.Front);
        glBindVertexArray(mouseVAO);
        glDrawArrays(GL_TRIANGLES, 0, 18);

		//检查调用事件，交换缓存
		//双缓冲：前面缓存一张画面用于显示，后台会额外缓存另一张，当第一张结束，直接交换，效果上便不会有重新逐行绘制带来的闪屏。
		glfwSwapBuffers(window.getGLFWwindow());//交换颜色缓存
		glfwPollEvents();//检查鼠标，键盘，更新窗口状态，并调用对应的回调函数。
    }
    //delete
    glDeleteVertexArrays(1, &earthVAO);
    glDeleteVertexArrays(1, &moonVAO);
    glDeleteVertexArrays(1, &sunVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VBOm);
    glDeleteBuffers(1, &VBO1);
    glDeleteBuffers(1, &VBO2);
    //退出glfw
    glfwTerminate();
}

void App::processKeyInput(GLFWwindow* window)
{
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

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // y向下为正向

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void mouse_presscallback(GLFWwindow* window, int button, int action, int mods)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        float mindist = fmin(sundist, fmin(moondist, earthdist));
        if (sundir && sundist < 1.5 && sundist == mindist)
        {
            std::cout << "太阳" << std::endl;
        }
        if (earthdir && earthdist < 1.0 && earthdist == mindist)
        {
            std::cout << "地球" << std::endl;
        }
        if (moondir && moondist < 1.0 && moondist == mindist)
        {
            std::cout << "月亮" << std::endl;
        }
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

uint32_t loadTexture(char const* path)
{
    uint32_t textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

float getdist(Sphere& sphere)
{
    glm::vec3 line = sphere.core - camera.Position;
    glm::vec3 crossres = glm::cross(line, camera.Front);
    float crossmod = sqrt(pow(crossres.x, 2) + pow(crossres.y, 2) + pow(crossres.z, 2));
    float mod = sqrt(pow(camera.Front.x, 2) + pow(camera.Front.y, 2) + pow(camera.Front.z, 2));
    return crossmod / mod;
}

bool getdir(Sphere& sphere)
{
    glm::vec3 line = sphere.core - camera.Position;
    float res = line.x * camera.Front.x + line.y * camera.Front.y + line.z * camera.Front.z;
    if (res > 0)
        return true;
    else return false;
}

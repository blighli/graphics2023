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
#include <random>
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
glm::mat4 modelk[10];
glm::vec3 v(0.0f, 0.0f, 0.0f);
int axischoose = 0;
// 摄像头
Camera camera(glm::vec3(0.0f, 2.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool modeldao[10] ;
bool isPointOnLine(glm::vec3 a, glm::vec3 b, glm::vec3 x) {
    // 计算从点 a 到点 b 的向量
    glm::vec3 ab = b - a;

    // 检查 ab 与 x 是否共线，即叉积为零向量
    glm::vec3 cross_product = glm::cross(ab, glm::normalize(x));

    // 叉积为零向量时，两向量共线
    if (glm::length(cross_product) <= 0.01) {
        // 进一步检查点积，确保 ab 和 x 在同一方向
        float dot_product = glm::dot(ab, x);
        return dot_product >= 0; // 如果点积为正，或为零但 ab 也为零向量，则 b 在 a 沿 x 的线上
    }

    // 如果叉积非零，b 不在直线上
    return false;
}

int main()
{
    std::random_device rd;  // 可以用来作为随机数生成器的种子
    std::mt19937 gen(rd()); // 标准的mersenne_twister_engine
    std::uniform_real_distribution<float> dis(-2.0f, 2.0f);
    //模型初位置设置
    for (int i = 0; i < 10; ++i) {
        float randomX = dis(gen);
        float randomZ = dis(gen);
        modelk[i] = glm::translate(model, glm::vec3(randomX, 0.0f, randomZ)); // translate it down so it's at the center of the scene
        modelk[i] = glm::scale(modelk[i], glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down
    }
    //model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    /*-------------------初始化--------------------*/
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HeadShot", NULL, NULL);
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
    Shader UIShader("UI.vs", "UI.fs");
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
        glm::vec3(0.0f,  3.0f, 0.0f)
    };
    //地面顶点和索引
    float groundVertices[] = {
        // 位置             // 法线           // 纹理坐标
        -0.5f,  0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // 左上角
         0.5f,  0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // 右上角
         0.5f,  0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // 右下角
        -0.5f,  0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f  // 左下角
    };
    int groundIndices[] = {
        0, 1, 2, // 第一个三角形
        0, 2, 3  // 第二个三角形
    };
    //狙击枪准镜顶点数据
    float Gunvertices[] = {
        // 位置     // 纹理坐标
        -1.0f,  1.0f,  0.0f, 1.0f, // 左上角
        -1.0f, -1.0f,  0.0f, 0.0f, // 左下角
         1.0f, -1.0f,  1.0f, 0.0f, // 右下角
         1.0f,  1.0f,  1.0f, 1.0f  // 右上角
    };
    unsigned int Gunindices[] = {
    0, 1, 3, // 第一个三角形
    1, 2, 3  // 第二个三角形
    };
    Model ourModel("../model/nanosuit.obj");
    Model room("../model/ro.obj");
    /*-----------------顶点数组对象--------------------*/
    //正方体点光源
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

    //地面
    unsigned int GVAO, GVBO, GEBO;
    glGenVertexArrays(1, &GVAO);
    glGenBuffers(1, &GVBO);
    glGenBuffers(1, &GEBO);
    glBindVertexArray(GVAO);
    glBindBuffer(GL_ARRAY_BUFFER, GVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //狙击枪
    unsigned int GunVBO, GunVAO, GunEBO;
    glGenVertexArrays(1, &GunVAO);
    glGenBuffers(1, &GunVBO);
    glGenBuffers(1, &GunEBO);

    glBindVertexArray(GunVAO);

    glBindBuffer(GL_ARRAY_BUFFER, GunVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Gunvertices), Gunvertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GunEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Gunindices), Gunindices, GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    /*-----------------纹理--------------------*/
    unsigned int texture,texture2;
    int width1,width2, height1,height2, nrChannels1, nrChannels2;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data1 = stbi_load("../img/container.jpg", &width1, &height1, &nrChannels1, 0);
    unsigned char* data2 = stbi_load("../img/780.jpg", &width2, &height2, &nrChannels2, 0);
    // 创建纹理
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data1);
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    GLenum format;
    if (nrChannels2 == 1)
        format = GL_RED;
    else if (nrChannels2 == 3)
        format = GL_RGB;
    else if (nrChannels2 == 4)
        format = GL_RGBA;
    else
        format = GL_RGB;
    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width2, height2, 0, format, GL_UNSIGNED_BYTE, data2);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data2);
    glBindTexture(GL_TEXTURE_2D, 0);
    /*--------------------------------------------------*/
    // 着色器设置
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);

    //循环渲染
    while (!glfwWindowShouldClose(window))
    {
        bool gameover = true;
        for (int i = 0; i < 10; ++i) {
            if (modeldao[i] == true)
                continue;
            gameover = false;
        }

        glEnable(GL_DEPTH_TEST);
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
        //lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        //lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        //lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        //lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        //lightingShader.setFloat("pointLights[1].constant", 1.0f);
        //lightingShader.setFloat("pointLights[1].linear", 0.09f);
        //lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
        ////点光源3
        //lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        //lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        //lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        //lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        //lightingShader.setFloat("pointLights[2].constant", 1.0f);
        //lightingShader.setFloat("pointLights[2].linear", 0.09f);
        //lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
        ////点光源4
        //lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        //lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        //lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        //lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        //lightingShader.setFloat("pointLights[3].constant", 1.0f);
        //lightingShader.setFloat("pointLights[3].linear", 0.09f);
        //lightingShader.setFloat("pointLights[3].quadratic", 0.032f);

        //观察与投影
        
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setM4fv("projection", projection);
        lightingShader.setM4fv("view", view);

        //世界系矩阵
        //model = glm::translate(model, v * (10*deltaTime));
        //glm::vec3 modelPosition = glm::vec3(model[3]); // 提取模型的位置
        //glm::vec3 modelZAxis = glm::normalize(glm::vec3(model[2]));
        //glm::vec3 cameraPosition = modelPosition + (modelZAxis * 0.4f)+ glm::vec3(0.0f,0.3f,0.0f);
        //camera.setPosition(cameraPosition);
        for (int i = 0; i < 10; ++i) {
            lightingShader.setM4fv("model", modelk[i]);
            ourModel.Draw(lightingShader);
        };
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glm::mat4 ground = glm::scale(glm::mat4(1.0f),glm::vec3(5.0f, 5.0f, 5.0f));
        lightingShader.setM4fv("model", ground);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(GVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glm::mat4 ground1 = glm::translate(ground,glm::vec3(0.5f,0.5f,0.0f));
        ground1 = glm::rotate(ground1, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        lightingShader.setM4fv("model", ground1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glm::mat4 ground2 = glm::translate(ground, glm::vec3(-0.5f, 0.5f, 0.0f));
        ground2 = glm::rotate(ground2, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        lightingShader.setM4fv("model", ground2);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glm::mat4 ground3 = glm::translate(ground, glm::vec3(0.0f, 0.5f, -0.5f));
        ground3 = glm::rotate(ground3, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        lightingShader.setM4fv("model", ground3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glm::mat4 ground4 = glm::translate(ground, glm::vec3(0.0f, 1.0f, 0.0f));
        ground4 = glm::rotate(ground4, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        lightingShader.setM4fv("model", ground4);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glm::mat4 modelroom;
        modelroom = glm::mat4(1.0f);
        modelroom = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
        lightingShader.setM4fv("model", modelroom);
        //room.Draw(lightingShader);

        
        //光源着色器启用
        lightCubeShader.use();
        lightCubeShader.setM4fv("projection", projection);
        lightCubeShader.setM4fv("view", view);

        //光源绘制
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 1; i++)
        {
            glm::mat4 modellight;
            modellight = glm::mat4(1.0f);
            modellight = glm::translate(modellight, pointLightPositions[i]);
            modellight = glm::scale(modellight, glm::vec3(0.02f)); // Make it a smaller cube
            lightCubeShader.setM4fv("model", modellight);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        if (!Pattern) {
            glDisable(GL_DEPTH_TEST);
            UIShader.use();
            UIShader.setInt("texture1", 0);
            glm::mat4 orthoProjection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), static_cast<float>(SCR_HEIGHT), 0.0f, -1.0f, 1.0f);
            UIShader.setM4fv("projection", glm::mat4(1.0f));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture2);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBindVertexArray(GunVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glUseProgram(0);
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
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, -25.0f * deltaTime));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0f * deltaTime));
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
        //model = glm::translate(model, glm::vec3(-25.0f * deltaTime, 0.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
        //model = glm::translate(model, glm::vec3(25.0f * deltaTime, 0.0f, 0.0f));
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
 
    camera.ProcessMouseMovement(xoffset, yoffset);
    
}

//滚轮
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    //if (Pattern) {
        //camera.ProcessMouseScroll(static_cast<float>(yoffset));
   // }
    //else {
        //if (yoffset > 0)
           // model = glm::scale(model, glm::vec3(0.9f, 0.9f, 0.9f));
        //else
           // model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
    //}
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
void mouse_clickcallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (!Pattern) {
            for (int i = 0; i < 10; ++i) {
                glm::vec3 modelPosition = glm::vec3(modelk[i][3]);
                if (!modeldao[i] && isPointOnLine(camera.Position, modelPosition+glm::vec3(0.0f,0.14f,0.0f), camera.Front)) {
                    modelk[i] = glm::translate(modelk[i], glm::vec3(0.0f, 0.3f, 0.0f));
                    modelk[i] = glm::rotate(modelk[i], glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    modeldao[i] = true;
                }
            }
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        camera.kaijing();//随意参数
        Pattern = !Pattern;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        ;
}


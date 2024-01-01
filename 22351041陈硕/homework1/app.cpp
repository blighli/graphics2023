#include "app.h"
#include "camera.h"
//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//std
#include <stdexcept>
#include <array>
#include <chrono>
#include <iostream>

const float MAX_FRAME_TIME = 1;

App::App(){}

App::~App() {}

void App::run()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
    glViewport(0, 0, WIDTH, HEIGHT);
    //Camera camera;
    //camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
    //camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));
    auto currentTime = std::chrono::high_resolution_clock::now();
    Shader shader{ "vertshader.vert", "fragshader.frag" };
    // 设置顶点数据
    float vertices[] = {
        // 位置              // 颜色
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // left  
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // right 
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f  // top   
    };
    // 创建一个顶点数组对象VAO和一个顶点缓冲对象VBO
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // 绑定VAO，绑定VBO到GL_ARRAY_BUFFER目标上，绑定EBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 将顶点数组复制到缓冲中供OpenGL使用
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 设置顶点属性指针
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    while (!window.shouldClose())
    {
        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;
        //to avoid auto move when changing window size
        frameTime = glm::min(frameTime, MAX_FRAME_TIME);
        //检查按键是否按下
        window.processInput();
		//渲染指令
		//glClearColor设置清空屏幕所用的颜色----状态设置函数
		//glClear函数清除颜色缓冲之后，屏幕会填充为glClearColor中的颜色。---状态使用函数
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);	
		glClear(GL_COLOR_BUFFER_BIT);
        //draw
        shader.use();
        shader.setFloat("someUniform", 1.0f);   
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
		//检查调用事件，交换缓存
		//双缓冲：前面缓存一张画面用于显示，后台会额外缓存另一张，当第一张结束，直接交换，效果上便不会有重新逐行绘制带来的闪屏。
		glfwSwapBuffers(window.getGLFWwindow());//交换颜色缓存
		glfwPollEvents();//检查鼠标，键盘，更新窗口状态，并调用对应的回调函数。
    }
    //delete

    //退出glfw
    glfwTerminate();
}


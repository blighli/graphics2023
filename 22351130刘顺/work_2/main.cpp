#include <glad/glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "camera.h"
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "sphere.h"
#include <math.h>
using namespace std;
using namespace glm;

//帧时间控制
float deltaTime = 0.0f;	// 两帧间隔
float lastFrame = 0.0f; // 上一帧时间

// 借助camera类实现相机控制
Camera camera(vec3(0.0f, 1.0f, 15.0f));
float lastX = 1280 / 2.0f;
float lastY = 720 / 2.0f;
bool firstMouse = true;


//控制相机移动和esc退出
void processInput(GLFWwindow *window)
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
}


// 鼠标回调，移动摄像头视角
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
// 鼠标滑轮回调，缩放控制
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//将一些初始化进行二次封装
//纹理载入 利用stb_img库
unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        //绑定纹理
        glBindTexture(GL_TEXTURE_2D, textureID);
        //生成纹理
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        //设置重复
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //设置mipmap和线性插值方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //释放图像内存
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

GLFWwindow* init(){
    //初始化
    glfwInit();
    //设置版本为opengl3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //设置为核心模式
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //设置窗口
    GLFWwindow* window = glfwCreateWindow(1280, 720, "task2", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    //绑定一系列外部硬件事件
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //初始化glad
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glViewport(0, 0, 1280, 720);
    return window;
}




int main(){
    //初始化
    GLFWwindow* window= init();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //编译shader，借助shader类编译，不知道啥原因，写相对路径无法识别
    //无奈采用绝对路径方式
    Shader sunShader("D:\\研究生\\课程\\三维动画与交互\\graphics2023\\22351130刘顺\\work_2sourcecode\\work_2\\shader\\homework2_sun.vs","D:\\研究生\\课程\\三维动画与交互\\graphics2023\\22351130刘顺\\work_2\\shader\\homework2_sun.fs");
    Shader planetShader("D:\\研究生\\课程\\三维动画与交互\\graphics2023\\22351130刘顺\\work_2\\shader\\homework2_planet.vs","D:\\研究生\\课程\\三维动画与交互\\graphics2023\\22351130刘顺\\work_2\\shader\\homework2_planet.fs");

    //加载并激活纹理，同样用绝对路径，跟shader同问题
    unsigned int sunTexture = loadTexture("D:\\研究生\\课程\\三维动画与交互\\graphics2023\\22351130刘顺\\work_2\\img\\2k_sun.jpg");
    unsigned int moonTexture = loadTexture("D:\\研究生\\课程\\三维动画与交互\\graphics2023\\22351130刘顺\\work_2\\img\\2k_moon.jpg");
    unsigned int earthTexture = loadTexture("D:\\研究生\\课程\\三维动画与交互\\graphics2023\\22351130刘顺\\work_2\\img\\2k_earth_daymap.jpg");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sunTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, earthTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, moonTexture);

    //初始化球体对象，并使用默认sector、stack
    Sphere sphere = Sphere();

    //因为涉及到光照和纹理，因此还需要额外两个VBO
    unsigned int VAO,VBO,EBO,normalVBO, textureVBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    //取出vector对象的size和data来获得长度和数组
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphere.vertices.size() * sizeof(float), sphere.vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.indices.size() * sizeof(int), sphere.indices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &normalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, sphere.normals.size() * sizeof(float), sphere.normals.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &textureVBO);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glBufferData(GL_ARRAY_BUFFER, sphere.texcoords.size() * sizeof(float), sphere.texcoords.data(), GL_STATIC_DRAW);

    //链接属性
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    //解绑，防止对VAO的误操作
    glBindVertexArray(0);

    //定义星球属性
    float sunSize = 3,earthSize = 1,moonSize = 0.3;
    float sunEarthDistance = 10,earthMoonDistance = 2;

    //累加量
    int acc = 0;
    while(!glfwWindowShouldClose(window))
    {
        //获得每一帧的时间
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //接收按键操作
        processInput(window);
        //清除缓存
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        // 透视和投影矩阵
        mat4 projection = perspective(radians(camera.Zoom), (float)(1280.0f / 720.0f), 0.1f, 300.0f);
        mat4 view = camera.GetViewMatrix();
        mat4 model = mat4(1.0f);

        //太阳,自下而上执行变换
        sunShader.use();
        sunShader.setInt("texture1", 0);
        sunShader.setMat4("view", view);
        sunShader.setMat4("projection", projection);
        model = mat4(1.0f);
        model = rotate(model,radians((acc%1800)*0.2f),vec3(0.0f,1.0f,0.0f));
        model = scale(model,vec3(sunSize));
        model = translate(model,vec3(0.0f,0.0f,0.0f));
        sunShader.setMat4("model", model);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sphere.indices.size(), GL_UNSIGNED_INT, 0);

        //地球
        planetShader.use();
        planetShader.setInt("texture1",1);
        planetShader.setMat4("view",view);
        planetShader.setMat4("projection", projection);
        model = mat4(1.0f);
        //一开始地球月亮太阳在一条直线上
        model = rotate(model,radians((acc%900)*0.4f),vec3(0.0f,1.0f,0.0f));
        model = translate(model,vec3(sunEarthDistance,0,0));
//        model = translate(model,vec3(sunEarthDistance*cos((acc%36)*15.0f),0,sunEarthDistance*sin((acc%36)*15.0f)));
        model = rotate(model,radians((acc%1200)*0.3f),vec3(0.0f,1.0f,0.0f));
        model = scale(model,vec3(earthSize));
        sunShader.setMat4("model", model);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sphere.indices.size(), GL_UNSIGNED_INT, 0);

        //月亮
        planetShader.use();
        planetShader.setInt("texture1",2);
        planetShader.setMat4("view",view);
        planetShader.setMat4("projection", projection);
        model = mat4(1.0f);
        //一开始地球月亮太阳在一条直线上，月球先转小圈再转大圈
        model = rotate(model,radians((acc%900)*0.4f),vec3(0.0f,1.0f,0.0f));
        model = translate(model,vec3(sunEarthDistance,0,0));
        model = rotate(model,radians((acc%720)*0.5f),vec3(0.0f,1.0f,0.0f));
        model = translate(model,vec3(earthMoonDistance,0,0));
//        model = translate(model,vec3(sunEarthDistance*cos((acc%36)*15.0f),0,sunEarthDistance*sin((acc%36)*15.0f)));
        model = scale(model,vec3(moonSize));
        sunShader.setMat4("model", model);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sphere.indices.size(), GL_UNSIGNED_INT, 0);

        glfwPollEvents();
        glfwSwapBuffers(window);
        acc++;
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1,&EBO);
    glDeleteBuffers(1,&normalVBO);
    glDeleteBuffers(1,&textureVBO);
    glfwTerminate();
    return 0;
}


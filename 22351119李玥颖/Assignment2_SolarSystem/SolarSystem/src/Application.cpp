#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Shader.h"
#include "Sphere.h"
#include "Config.h"
#include "Camera.h"
#include "Renderer.h"
#include "Texture.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// camera
Camera camera(glm::vec3(0.0f, 5.0f, 35.0f));
float lastX = WindowWidth / 2.0f;
float lastY = WindowHeight / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

bool showSun;
bool showMoon;
bool showEarth;

glm::vec3 sunWorldPosition(0.0f,1.0f,0.0f);     // 太阳的世界坐标
glm::vec3 earthWorldPosition; // 地球的世界坐标
glm::vec3 moonWorldPosition;   // 月球的世界坐标

int main()
{
    // 1 initialize glfw and the windows
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2 create the window and check if success
    GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "Solar System", nullptr, nullptr);
    if (!window)
    {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
    }

    // 3 set context and click
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSwapInterval(1);

    // 4 initialize glad and load pointer
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
    }

    // 5 set a sphere and get all vertices of sphere and EBO array
    Sphere mySphere(40);
    std::vector<float> sphereVertices = mySphere.getVertices();
    std::vector<unsigned int> sphereIndices = mySphere.getIndices();

    {
        // 6 create VAO、VBO、EBO
        VertexArray va;
        VertexBuffer vb(&sphereVertices[0], sphereVertices.size() * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(2);
        va.AddBuffer(layout);

        IndexBuffer ib(&sphereIndices[0], sphereIndices.size());//放在最后

        // 7 set up deep test
        glEnable(GL_DEPTH_TEST);

        // 8 set shader and texture
        Shader shader("res/shader/task3.vs", "res/shader/task3.fs");
        
        // add light shader for earth
        Shader lightShader("res/shader/earth.vs", "res/shader/earth.fs");

        shader.Bind();
        shader.setInt("v_TexCoord", 0);

        lightShader.Bind();
        lightShader.setInt("material.diffuse", 0);

        Texture textureSun("res/textures/sun1.jpg");
        Texture textureEarth("res/textures/earth.jpg");
        Texture textureMoon("res/textures/moon.jpg");

        // 9 unbind VAO、VBO、EBO
        vb.Unbind();
        va.Unbind();
        ib.Unbind();
        shader.Unbind();
        lightShader.Unbind();

        // 10 shader loop
        Renderer render;
    
        //glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
        glm::vec3 lightPos(0.0f, 1.0f, 0.0f);
        while (!glfwWindowShouldClose(window))
        {
            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            processInput(window);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)WindowWidth / (float)WindowHeight, 0.1f, 100.0f);

            va.Bind();
            {
                // Sun
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::rotate(glm::mat4(1.0f), -(float)glfwGetTime() / 5, glm::vec3(0.0f, 1.0f, 0.0f));
                model = scale(model, glm::vec3(sunScale, sunScale, sunScale));
                glm::mat4 mvp = proj * view * model;

                textureSun.Bind();
                shader.Bind();
                shader.setMat4("u_MVP", mvp);

                render.Draw(mySphere.getNumIndices());
                sunWorldPosition = glm::vec3(0.0f, 1.0f, 0.0f);     // 太阳的世界坐标
                //
            }
      
            {// Earth
        
                glm::mat4 model = glm::mat4(1.0f);
                // 公转
                model = glm::rotate(model, (float)glfwGetTime() / 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::translate(model, glm::vec3(SunEarthDistance, .0f, .0f));
                // 抵消公转对自身倾斜方向的影响，保证公转后 仍然向右倾斜
                model = glm::rotate(model, -(float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::rotate(model, -glm::radians(ErothAxialAngle), glm::vec3(0.0, 0.0, 1.0));
                // 自转
                model = glm::rotate(model, -(float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
                glm::mat4 mvp = proj * view * model;

                textureEarth.Bind();
                //shader.Bind();
                lightShader.Bind();

                lightShader.setVec3("light.position", lightPos);
                lightShader.setVec3("viewPos", camera.Position);

                // light properties
                lightShader.setVec3("light.ambient", 0.45f, 0.45f, 0.45f);
                lightShader.setVec3("light.diffuse", 0.6f, 0.6f, 0.6f);
                lightShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

                // material properties
                lightShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
                lightShader.setFloat("material.shininess", 64.0f); 
                lightShader.setMat4("u_MVP", mvp);
                lightShader.setMat4("model", model);

                render.Draw(mySphere.getNumIndices());
                
                // 将时间单位转变为弧度，所以我们需要乘以2π
                float radian = (float)glfwGetTime() / 1.5f * 2.0f * 3.1415926f;

                // 计算出x和z的值
                float x = SunEarthDistance * cos(radian);
                float z = SunEarthDistance * sin(radian);
                earthWorldPosition = glm::vec3(x, 0, z);
            }

            { // Moon
        
                glm::mat4 model = glm::mat4(1.0f);
                // 地日公转
                model = glm::rotate(model, (float)glfwGetTime() / 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::translate(model, glm::vec3(SunEarthDistance, .0f, .0f));
                // 月球公转
                model = glm::rotate(model, (float)glfwGetTime() * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::translate(model, glm::vec3(MoonEarthDistance, 0.0, 0.0));
                // 月球自转
                model = glm::rotate(model, -(float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
                model = scale(model, glm::vec3(moonScale, moonScale, moonScale));
                glm::mat4 mvp = proj * view * model;

                /*textureMoon.Bind();
                shader.Bind();
                shader.setMat4("u_MVP", mvp);*/
                textureMoon.Bind();
                //shader.Bind();
                lightShader.Bind();

                lightShader.setVec3("light.position", lightPos);
                lightShader.setVec3("viewPos", camera.Position);

                // light properties
                lightShader.setVec3("light.ambient", 0.45f, 0.45f, 0.45f);
                lightShader.setVec3("light.diffuse", 0.6f, 0.6f, 0.6f);
                lightShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

                // material properties
                lightShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
                lightShader.setFloat("material.shininess", 64.0f);
                lightShader.setMat4("u_MVP", mvp);
                lightShader.setMat4("model", model);


                render.Draw(mySphere.getNumIndices());
                
                // 时间转为弧度
                float radian = (float)glfwGetTime() * 2.0f * 3.1415926f;

                // 计算月球相对于地球的位置
                float x = MoonEarthDistance * cos(radian);
                float z = MoonEarthDistance * sin(radian);

                // 将月球相对于地球的位置加到地球的位置上，得到月球的世界坐标
                moonWorldPosition = earthWorldPosition + glm::vec3(x, 0, z);
            
            }
          

            if (showSun)
            {
                //renderText("Earth", 1.f, 1.f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            }

            if (showEarth)
            {
                // 绘制地球
                // ...
            }

            if (showMoon)
            {
                // 绘制月亮
                // ...
            }
            glfwSwapBuffers(window);
            glfwPollEvents();
        } // shader loop end
    }

    glfwTerminate();
    return 0;
}

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
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

// 这是一个全局矩阵，可以保存你的相机的 projection * view 矩阵
glm::mat4 globalMatrix;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // 检查是否左键发生了按下行为
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        glm::mat4 viewMatrix = camera.GetViewMatrix();
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.Zoom), (float)WindowWidth / (float)WindowHeight, 0.1f, 100.0f);

        // 获取鼠标位置
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // 屏幕坐标到NDC转换
        float x = (2.0f * xpos) / WindowWidth - 1.0f;
        float y = 1.0f - (2.0f * ypos) / WindowHeight;
        float z = 1.0f;
        glm::vec3 ray_nds = glm::vec3(x, y, z); // 转化为NDC

        // NDC到裁剪空间的转换
        glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);

        // 裁剪空间到眼睛空间的转换
        glm::vec4 ray_eye = glm::inverse(projectionMatrix) * ray_clip; 
        ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0);
        glm::vec3 ray_wor = glm::vec3(glm::inverse(viewMatrix) * ray_eye);
        ray_wor = glm::normalize(ray_wor);
        
        std::cout << "Mouse Clicked at: " << ray_wor[0] << ", " << ray_wor[1] << "," << ray_wor[2] << std::endl;
        std::cout << "Sun at: " << sunWorldPosition[0] << "," << sunWorldPosition[1] << "," << sunWorldPosition[2] << std::endl;
        std::cout << "Earth at: " << earthWorldPosition[0] << "," << earthWorldPosition[1] << "," << earthWorldPosition[2] << std::endl;
        std::cout << "Moon at: " << moonWorldPosition[0] << "," << moonWorldPosition[1] << "," << moonWorldPosition[2] << std::endl;
                         
        // 核对三个世界坐标求解是否正确，核对半径是否正确

        float dist = glm::distance(earthWorldPosition, ray_wor);
        if (dist < (EAR_RADIUS/2)) { std::cout << "sun yes" << dist << std::endl; }
        else { std::cout << "sun no" << std::endl; }
    }
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <sstream>
#include "shader.h"
#include "camera.h"
#include "Model.h"
#include "stb_image.h"
#include <vector>
#include <iostream>
using namespace std;

#define numVAOs 1
#define numVBOs 3

GLuint vao[numVAOs];
GLuint vao2[numVAOs];
GLuint vbo[numVBOs];
GLuint vbo2[numVBOs];

Imported myModel("bunny.obj");

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
// settings
const unsigned int SCR_WIDTH = 2400;
const unsigned int SCR_HEIGHT = 1800;

//缩放参数
float s_size= 0.3f;//大小

//平移参数
float front_back = 0;
float front_back2 = 0;
float left_right = 0;
float left_right2 = 2.0;
float up_down = 0;
float up_down2 = 0;

//线框图转换标志
int line_flag = 0;

// camera视角变化类的引入
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
static const float pai = 3.1415926f;
static GLfloat _rotate_z = 0.0f;
float toRadians(float degrees)
{
    return (degrees * 2.f * pai) / 360.f;
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();//初始化GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//OpenGL主版本号 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//OpenGL副版本号 .3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//OpenGL模式 OpenGL核心模式

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);//窗口宽、高、标题
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    //各类主进程函数的调用，比如鼠标和键盘监听等等。
    glfwMakeContextCurrent(window);//让当前窗口的环境在当前线程上成为当前环境
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//监听窗口大小变化
    glfwSetCursorPosCallback(window, mouse_callback);//鼠标监听
    glfwSetScrollCallback(window, scroll_callback);//滚轮
    glfwSetKeyCallback(window, key_callback);//键盘监听

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("1.colors.vert", "1.colors.frag");

    // 调用设定顶点数据函数
    std::vector<glm::vec3> vert = myModel.getVertices();
    std::vector<glm::vec3> norm = myModel.getNormals();


    std::vector<float> pvalues;
    std::vector<float> nvalues;


    for (int i = 0; i < myModel.getNumVertices(); i++) {
        pvalues.push_back((vert[i]).x);
        pvalues.push_back((vert[i]).y);
        pvalues.push_back((vert[i]).z);
        nvalues.push_back((norm[i]).x);
        nvalues.push_back((norm[i]).y);
        nvalues.push_back((norm[i]).z);
    }

    //把数据存入缓存中
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);
    //-------------------------------

    int width, height, nrChannels;
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load("fur.tga", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        //线框图转换处理判断
        if (line_flag == 0) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // render
        // ------
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto timeControl = (float)glfwGetTime();//定义timeControl系统时间函数
        float radius = 10.0f;
        float camX = sin(timeControl) * radius;
        float camZ = cos(timeControl) * radius;//设置X、Z轴随时间的三角函数，实现光源的旋转
        glm::vec3 lightPos(camX, 0.0f, camZ);//设置光源三维向量的旋转
        glm::vec3 lightPos_pro(-camX, 0.0f, -camZ);//设置光源三维向量的旋转

        lightingShader.use();
        lightingShader.setVec3("objectColor", 0.788f, 0.459f, 0.525f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("lightPos_pro", lightPos_pro);
        lightingShader.setVec3("viewPos", camera.Position);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        // world transformation
        glm::mat4 model = glm::mat4(1.0f);

        //缩放
        model = glm::scale(model, glm::vec3(s_size, s_size, s_size));
        //平移
        model = glm::translate(model, glm::vec3(left_right, up_down, front_back));
        //旋转
        model = glm::rotate(model, toRadians(_rotate_z), glm::vec3(0.f, 1.f, 0.f));
        lightingShader.setMat4("model", model);
        glBindVertexArray(vao[0]);
        glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());
        glBindTexture(GL_TEXTURE_2D, texture);

        glm::mat4 projection2 = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.3f, 100.0f);
        glm::mat4 view2 = camera.GetViewMatrix();
        lightingShader.setMat4("projection2", projection2);
        lightingShader.setMat4("view2", view2);

        glBindVertexArray(vao2[0]);
        glEnable(GL_DEPTH_TEST);//深度测试
        glDepthFunc(GL_LEQUAL);//深度比较函数，深度小或相等的时候也渲染。

        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        _rotate_z = _rotate_z + 0.1f;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = (float)xpos;
    lastY = (float)ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Z) s_size += 0.03;//模型放大

    if (key == GLFW_KEY_X) s_size -= 0.03;//模型缩小

    if (key == GLFW_KEY_F) front_back += 0.3;//模型前移

    if (key == GLFW_KEY_G) front_back -= 0.3;//模型后移

    if (key == GLFW_KEY_J) left_right += 0.3;//模型右移

    if (key == GLFW_KEY_H) left_right -= 0.3;//模型左移

    if (key == GLFW_KEY_K) up_down += 0.3;//模型上移

    if (key == GLFW_KEY_L) up_down -= 0.3;//模型下移

    if (key == GLFW_KEY_C) line_flag = 0;//转换为面图

    if (key == GLFW_KEY_V) line_flag = 1;//转换为线框图
}

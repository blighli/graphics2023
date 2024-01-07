// Standard Headers
#include <iostream>
#include <filesystem>

// 3rd Party Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Project include
#include "Shader.h"
#include "Camera.h"
#include "Sphere.h"
#include "Texture.h"
#include "config.h"

// M&K 输入回调函数声明
void processInput(GLFWwindow *window); // 键盘输入
void framebuffer_size_callback(GLFWwindow* window, int width, int height); // 回调函数原型声明
void mouse_callback(GLFWwindow* window, double xpos, double ypos); // 鼠标移动
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset); // 鼠标滚轮

// 窗口大小
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 计时
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

// 相机参数
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f)); // 相机初始位置
float lastX = SCR_WIDTH / 2.0f; // 鼠标上一帧坐标，初始位置设为屏幕中心
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// 太阳位置
glm::vec3 sunPos(0.0f, 0.0f, 0.0f);

// 球体剖分精度
const int precision = 40;

int main()
{
    //初始化GLFW
    glfwInit();
    //配置GLFW input: {参数名, 参数值} return: bool
    //设置程序使用OpenGL的主版本号(Major)和次版本号(Minor)，需要显卡支持gl_ver3.3+
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //设置 GLFW 使用核心模式(Core-profile)，意味着我们只能使用OpenGL功能的一个子集（没有我们已不再需要的向后兼容特性）
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //MacOS特供
#endif
//------------------------------------------------------------------------------------//
    //创建一个窗口对象
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GyeongYun's solar system", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    //通知GLFW将我们窗口的上下文设置为当前线程的主上下文
    glfwMakeContextCurrent(window);
    //对窗口注册一个回调函数,每当窗口改变大小，GLFW会调用这个函数并填充相应的参数供你处理
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 对鼠标注册回调函数，鼠标/滚轮一移动mouse_callback函数就会被调用
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // 设置捕捉光标，不让鼠标离开窗口
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//------------------------------------------------------------------------------------//
    //初始化GLAD用来管理OpenGL的函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    std::cout << "Device OpenGL version: " << glGetString(GL_VERSION) << std::endl;
//------------------------------------------------------------------------------------//

//------------------------------------------------------------------------------------//
    // 从文件加载并编译着色器
    Shader planetShader(
        (std::filesystem::path(PROJECT_SOURCE_DIR)/"res"/"shaders"/"planet.vs").generic_string(),
        (std::filesystem::path(PROJECT_SOURCE_DIR)/"res"/"shaders"/"planet.fs").generic_string()
        );
    Shader stellarShader(
        (std::filesystem::path(PROJECT_SOURCE_DIR)/"res"/"shaders"/"stellar.vs").generic_string(),
        (std::filesystem::path(PROJECT_SOURCE_DIR)/"res"/"shaders"/"stellar.fs").generic_string()
        );
//----------------------------------------顶点数据-------------------------------------------------
    // 输入一些顶点数据，用于渲染一个球体
    Sphere mySphere(precision);
    std::vector<float> sphereVertices = mySphere.getVertices();
    std::vector<unsigned int> sphereIndices = mySphere.getIndices();


    // 通过顶点缓冲对象(Vertex Buffer Objects, VBO)和顶点数组对象(Vertex Array Object, VAO)管理这个内存
    unsigned int VAO;
    // 生成顶点数组对象，VAO
    glGenVertexArrays(1, &VAO);
    // 绑定VAO缓冲到相应的缓冲目标上
    glBindVertexArray(VAO);

    // 使用glGenBuffers函数和一个缓冲ID生成一个VBO对象
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    // 使用glBindBuffer函数把新创建的缓冲绑定到GL_ARRAY_BUFFER目标上
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 调用glBufferData函数，它会把之前定义的顶点数据复制到缓冲的内存中
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

    // EBO
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(float), &sphereIndices[0], GL_STATIC_DRAW);



    // 链接顶点属性，使用glVertexAttribPointer函数告诉OpenGL该如何解析顶点数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 法向属性，此处的法向量直接使用顶点坐标，因为球体的每个顶点都是从球心发出的，在精度高的情况下可以近似为球体表面的法向量
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    // 链接贴图属性
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // 解绑VAO, VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



//----------------------------------------顶点数据-------------------------------------------------


//----------------------------------------生成纹理-------------------------------------------------
    Texture earthTexture((std::filesystem::path(PROJECT_SOURCE_DIR)/"res"/"textures"/"earth.jpg").generic_string());
    Texture moonTexture((std::filesystem::path(PROJECT_SOURCE_DIR)/"res"/"textures"/"moon.jpg").generic_string());
    Texture sunTexture((std::filesystem::path(PROJECT_SOURCE_DIR)/"res"/"textures"/"sun.jpg").generic_string());

    planetShader.Bind();
    planetShader.setInt("Texture", 0);
    stellarShader.Bind();
    stellarShader.setInt("Texture", 0);
//----------------------------------------生成纹理-------------------------------------------------

    // 启用 OpenGL 功能：深度测试（Z buffer）
    glEnable(GL_DEPTH_TEST);

    // 开启面剔除 (只展示一个面，减少 3D 效果，不会有重合)
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK); // 面剔除模式：剔除背面

    // 使用线框模式绘制
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

//----------------------------------------渲染循环-------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // 计时
        float currentFrame = static_cast<float>(glfwGetTime()); // double 类型转换为 float
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 输入
        processInput(window);

        // 渲染指令
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // 刷新屏幕所使用的背景颜色
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 同时清空颜色和Z缓存

        // view/projection/world transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f); // init as use


        //------------------Sun------------------//
        sunTexture.Bind();
        stellarShader.Bind();
        stellarShader.setMat4("projection", projection);
        stellarShader.setMat4("view", view);
        stellarShader.setMat4("model", model);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, precision * precision * 6, GL_UNSIGNED_INT, 0);

        //------------------Earth------------------//
        // 使用Texture类中的纹理单元绑定纹理
        earthTexture.Bind();
        // 激活着色器
        planetShader.Bind(); // 等同shader.use()
        // 地球公转
        glm::mat4 earth_model = glm::rotate(model, (float)glfwGetTime() / 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
        earth_model = glm::translate(earth_model, glm::vec3(5.0f, 0.0f, 0.0f));
        earth_model = glm::scale(earth_model, glm::vec3(0.4f));
        planetShader.setMat4("projection", projection);
        planetShader.setMat4("view", view);
        planetShader.setMat4("model", earth_model);
        planetShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
        planetShader.setVec3("lightPos", sunPos);
        planetShader.setVec3("viewPos", camera.Position);
        // render the sphere
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, precision * precision * 6, GL_UNSIGNED_INT, 0);

        //------------------Moon------------------//
        moonTexture.Bind();
        planetShader.Bind();
        // 地日公转
        glm::mat4 moon_model = glm::rotate(model, (float)glfwGetTime() / 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
        moon_model = glm::translate(moon_model, glm::vec3(5.0f, 0.0f, 0.0f));
        // 月球公转
        moon_model = glm::rotate(moon_model, (float)glfwGetTime() * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        moon_model = glm::translate(moon_model, glm::vec3(1.0f, 0.0f, 0.0f));
        moon_model = glm::scale(moon_model, glm::vec3(0.2f));
        planetShader.setMat4("projection", projection);
        planetShader.setMat4("view", view);
        planetShader.setMat4("model", moon_model);
        planetShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
        planetShader.setVec3("lightPos", sunPos);
        planetShader.setVec3("viewPos", camera.Position);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, precision * precision * 6, GL_UNSIGNED_INT, 0);

        // 检查并调用事件，交换缓冲 (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window); //检查触发事件
        glfwPollEvents(); //交换颜色缓冲
    }
//----------------------------------------渲染循环-------------------------------------------------

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    //释放/删除之前的分配的所有资源
    glfwTerminate();
    return 0;
}

//输入控制，检查用户是否按下了返回键(Esc)
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
    // std::cout<<"camera position: "<<camera.Position.x<<", "<<camera.Position.y<<", "<<camera.Position.z<<std::endl;
}

// 当用户改变窗口的大小的时候，视口也应该被调整
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // 注意：对于视网膜(Retina)显示屏，width和height都会明显比原输入值更高一点。
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
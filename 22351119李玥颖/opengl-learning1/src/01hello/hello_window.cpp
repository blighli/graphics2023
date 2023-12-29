#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

using namespace std;

// 函数声明
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// 定义常量
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int hello_window(){
    // 1.initialize the window
    glfwInit(); // 初始化glfw
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 配置option
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Apple MAC
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // 2.create a window object
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL); // 创建一个window
    if(window == NULL){ // 测试是否创建成功
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // 创建上下文环境
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // view to display with callback function


    // 3.initialize glad: load all OpenGL function pointers
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){ // 加载gl函数的操作系统指针
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // 4.ready to engine rendering (not quit immediately)
    while(!glfwWindowShouldClose(window)){ // 验证是否点击关闭
        // input
        processInput(window); // 检查是否按下退出

        // render commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 颜色&透明度
        glClear(GL_COLOR_BUFFER_BIT); // 清颜色

        // check and call events and swap the buffers
        glfwSwapBuffers(window); // 展示颜色像素
        glfwPollEvents(); // 检查是否有事件触发
    }

    // 5.clean all resources
    glfwTerminate();
    cout << "step6 done" << endl;
    return 0;
}

//  glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height); // 左下和右上点
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // 检查是否按到esc退出按钮
        glfwSetWindowShouldClose(window, true); // 关闭界面
}
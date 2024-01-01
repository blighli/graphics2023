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

// Vertex shader & fragment shader and compile them
const char *vertexShaderSource = "#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main(){\n"
    "gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\n";
const char *fragmentShaderSource = "#version 460 core\n"
    "out vec4 FragColor;\n"
    "void main(){\n"
        "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n";


int rectangle(){
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

    // 4.build and compile our shader program
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check compile if success
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check compile if success
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram(); // return ID
    // attach compiled shaders to program obj and link
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check if success
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }
    // delete the linked shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    

    // 5.set up vertex data and buffers(NDC from -1 to 1)
    float vertices[] = {
         0.5f, 0.5f, 0.0f, // right
         0.5f, -0.5f, 0.0f, // right
        -0.5f, -0.5f, 0.0f, // left
        -0.5f,  0.5f, 0.0f  // right
         //0.0f, 0.5f, 0.0f //top
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    // 生成一个bufferID
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); 
    glGenBuffers(1, &EBO);
    // bind Vertex Array obj
    glBindVertexArray(VAO); 
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0); // to interpret
    glEnableVertexAttribArray(0);
    // unbind buffer & rebind VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);//VAO

    // 6.ready to engine rendering (not quit immediately)
    while(!glfwWindowShouldClose(window)){ // 验证是否点击关闭
        // input
        processInput(window); // 检查是否按下退出

        // render commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 颜色&透明度
        glClear(GL_COLOR_BUFFER_BIT); // 清颜色

        // to draw our triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_TRIANGLES, 0, 3);

        // check and call events and swap the buffers
        glfwSwapBuffers(window); // 展示颜色像素
        glfwPollEvents(); // 检查是否有事件触发
    }

    // 7.de-allocate all resources and clean all resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate(); // clear terminal
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
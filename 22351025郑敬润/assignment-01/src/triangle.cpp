#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);//回调函数原型声明
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 顶点着色器的源代码硬编码在代码文件顶部的C风格字符串中
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
// 片段着色器
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

int main()
{
    //初始化GLFW
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //创建一个窗口对象
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello OpenGL!", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    //通知GLFW将我们窗口的上下文设置为当前线程的主上下文
    glfwMakeContextCurrent(window);
    //对窗口注册一个回调函数,每当窗口改变大小，GLFW会调用这个函数并填充相应的参数供你处理
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //初始化GLAD用来管理OpenGL的函数指针
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


//----------------------------------------顶点着色器-------------------------------------------------
    // 创建一个着色器对象，把需要创建的着色器类型以参数形式提供给glCreateShader，传递的参数是GL_VERTEX_SHADER
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // 把这个着色器源码附加到着色器对象上，然后编译它
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // 检查编译是否成功
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
//----------------------------------------顶点着色器-------------------------------------------------


//----------------------------------------片段着色器-------------------------------------------------
    // 定义片段着色器Fragment Shade，所做的是计算像素最后的颜色输出，初始化操作同顶点着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // 检查编译是否成功
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
//----------------------------------------片段着色器-------------------------------------------------


//----------------------------------------着色器程序-------------------------------------------------
    // 着色器程序对象(Shader Program Object)是多个着色器合并之后并最终链接完成的版本
    // 如果要使用刚才编译的着色器我们必须把它们链接(Link)为一个着色器程序对象，然后在渲染对象的时候激活这个着色器程序
    // 创建一个程序对象：
    unsigned int shaderProgram = glCreateProgram();
    // 把之前编译的着色器附加到程序对象上，然后用glLinkProgram链接
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // 检测链接着色器程序是否失败
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    //把着色器对象链接到程序对象以后，记得删除着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
//----------------------------------------着色器程序-------------------------------------------------



//----------------------------------------顶点数据-------------------------------------------------
    // 输入一些顶点数据，用于渲染一个三角形
    // 注意，OpenGL仅当3D坐标在3个轴（x、y和z）上-1.0到1.0的范围内时才处理它。所有在这个范围内的坐标叫做标准化设备坐标（Normalized Device Coordinates，NDC）
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    }; 

    // 通过顶点缓冲对象(Vertex Buffer Objects, VBO)和顶点数组对象(Vertex Array Object, VAO)管理这个内存
    unsigned int VBO, VAO;
    // 使用glGenBuffers函数和一个缓冲ID生成一个VBO对象
    glGenBuffers(1, &VBO);
    // 使用glBindBuffer函数把新创建的缓冲绑定到GL_ARRAY_BUFFER目标上
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 调用glBufferData函数，它会把之前定义的顶点数据复制到缓冲的内存中
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 生成顶点数组对象，VAO
    glGenVertexArrays(1, &VAO);
    // 绑定VAO缓冲到相应的缓冲目标上
    glBindVertexArray(VAO);
//----------------------------------------顶点数据-------------------------------------------------


    // 链接顶点属性，使用glVertexAttribPointer函数告诉OpenGL该如何解析顶点数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

//----------------------------------------渲染循环-------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // 输入
        processInput(window);

        // 渲染指令
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // no need to unbind it every time 
 
        // 检查并调用事件，交换缓冲 (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window); //检查触发事件
        glfwPollEvents(); //交换颜色缓冲
    }
//----------------------------------------渲染循环-------------------------------------------------

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    //释放/删除之前的分配的所有资源
    glfwTerminate();
    return 0;
}

//输入控制，检查用户是否按下了返回键(Esc)
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// 当用户改变窗口的大小的时候，视口也应该被调整
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // 注意：对于视网膜(Retina)显示屏，width和height都会明显比原输入值更高一点。
    glViewport(0, 0, width, height);
}
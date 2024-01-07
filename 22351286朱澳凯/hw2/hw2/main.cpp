#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <camera.h>
#include <iostream>
#include "planet.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct obj
{
    glm::vec4 o;
    double r;
    const char* name;
}objs[3];

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "hw2", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // 鼠标显示
    glfwMakeContextCurrent(window);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);


    Planet* sun = new Planet(1.0);
    sun->Init("./images/sun.bmp");

    Planet* moon = new Planet(0.2);
    moon->Init("./images/moon.bmp");

    Planet* earth = new Planet(0.5);
    earth->Init("./images/earth.bmp");

    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // configure transformation matrices
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();;
        // 让太阳绕 Y 轴旋转
        float sunRotationSpeed = 24.0f;  // 调整太阳自转速度
        float earthRotationSpeed = 24.0f;  // 调整地球自转速度
        float earthOrbitSpeed = 2.0f;  // 调整地球公转速度
        float moonRotationSpeed = 24.0f;  // 调整月球自转速度
        float moonOrbitSpeed = 20.0f;  // 调整月球公转速度

        // 绘制太阳
        glm::mat4 sunModel = glm::rotate(glm::mat4(1.0f), glm::radians(sunRotationSpeed * currentFrame), glm::vec3(0.0f, 1.0f, 0.0f));
        sun->model = sunModel;
        sun->draw(projection, view, camera.Position, true);
        objs[0] = { sunModel * glm::vec4(0,0,0,1.0f),sun->getR(), "sun"};

        // 绘制地球
        glm::mat4 earthOrbit = glm::rotate(glm::mat4(1.0f), glm::radians(earthOrbitSpeed * currentFrame), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 earthModeltr = glm::translate(earthOrbit, glm::vec3(5, 0, 0));
        glm::mat4 earthModel = glm::rotate(earthModeltr, glm::radians(earthRotationSpeed * currentFrame), glm::vec3(0.0f, 1.0f, 0.0f));  // 自转
        objs[1] = { earthModel * glm::vec4(0,0,0,1.0f) ,earth->getR(), "earth"};
        earth->model = earthModel;
        earth->draw(projection, view, camera.Position);

        //// 绘制月球

        glm::mat4 moonOrbit = glm::rotate(glm::mat4(1.0f), glm::radians(moonOrbitSpeed * currentFrame), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 moonModel = glm::translate(moonOrbit, glm::vec3(1.0f, 0.0f, 0.0f));
        moonModel = earthModeltr * moonModel;
        moonModel = glm::rotate(moonModel,glm::radians(earthRotationSpeed * currentFrame), glm::vec3(0.0f, 1.0f, 0.0f));  // 自转
        objs[2] = { moonModel * glm::vec4(0,0,0,1.0f) ,moon->getR(), "moon"};
        moon->model = moonModel;
        moon->draw(projection, view, camera.Position);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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

bool raySphereIntersect(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 center, float radius)
{
    // 将射线的方向向量标准化
    rayDirection = glm::normalize(rayDirection);

    // 计算球心到射线起点的向量
    glm::vec3 oc = rayOrigin - center;

    // 计算射线的a、b和c系数
    float a = glm::dot(rayDirection, rayDirection);
    float b = 2.0f * glm::dot(oc, rayDirection);
    float c = glm::dot(oc, oc) - radius * radius;

    // 计算判别式
    float discriminant = b * b - 4 * a * c;

    // 如果判别式小于0，说明没有交点
    if (discriminant < 0)
    {
        return false;
    }
    else
    {
        // 计算两个交点的参数
        float t1 = (-b - sqrt(discriminant)) / (2.0f * a);
        float t2 = (-b + sqrt(discriminant)) / (2.0f * a);

        // 如果两个参数有一个大于0，说明有交点
        return t1 > 0 || t2 > 0;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        float x = (2.0f * mouse_x) / SCR_WIDTH - 1.0f;
        float y = 1.0f - (2.0f * mouse_y) / SCR_HEIGHT;
        float z = 1.0f;
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix(); 
        glm::vec3 ray_nds = glm::vec3(x, y, z);
        glm::vec4 ray_clip = glm::vec4(ray_nds, 1.0);
        glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
        glm::vec4 ray_world = glm::inverse(view) * ray_eye;
        if (ray_world.w != 0.0)
        {
            ray_world.x /= ray_world.w;
            ray_world.y /= ray_world.w;
            ray_world.z /= ray_world.w;
        }

        glm::vec3 ray_dir = glm::normalize(glm::vec3(ray_world) - camera.Position);
        
        for (int i = 0; i < 3; ++i) {
            if (raySphereIntersect(camera.Position, ray_dir, objs[i].o, objs[i].r)) {
                glfwSetWindowTitle(window, objs[i].name);
                return;
            }
        }
        glfwSetWindowTitle(window, "hw2");
    }
}

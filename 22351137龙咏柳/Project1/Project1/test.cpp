#include <GLFW/glfw3.h>

int main(void){
    GLFWwindow* window; 
    if (!glfwInit())
        return -1; 
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)    {
        glfwTerminate();
        return -1;    } 
    glfwMakeContextCurrent(window); 
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT); 
        glfwSwapBuffers(window); 
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

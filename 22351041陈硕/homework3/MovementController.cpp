#include "MovementController.h"

//std
#include <limits>

static bool firstMouse;
static float lastX;
static float lastY;
float deltaX, deltaY, scrollY;
bool MovementController::state = false;

void MovementController::ProcessMouseMovement(GLFWwindow* window, float dt, Camera& camera)
{
    camera.ProcessMouseMovement(deltaX, deltaY);
    deltaX = 0, deltaY = 0;
    camera.ProcessMouseScroll(static_cast<float>(scrollY));
    scrollY = 0;
}

void MovementController::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    deltaX = xpos - lastX;
    deltaY = lastY - ypos; // y向下为正向

    lastX = xpos;
    lastY = ypos;
}

void MovementController::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        state = true;
    else
        state = false;
}

void MovementController::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    scrollY = yoffset;
}

void MovementController::initMouse(GLFWwindow* window)
{
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
}

void MovementController::moveInPlaneXZ(GLFWwindow* window, float dt, Camera& camera)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, dt);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, dt);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, dt);
}

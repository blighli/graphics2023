#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// 相机移动方向
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// 摄像机参数
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 25.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    int mode;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // 获取V矩阵
    glm::mat4 GetViewMatrix();

    // 键盘输入响应处理
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    // 鼠标指针输入响应处理
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    // 鼠标滚轮输入响应处理
    void ProcessMouseScroll(float yoffset, float deltaTime);

private:
    // 更新相机参数
    void updateCameraVectors();
};
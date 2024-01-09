#pragma once
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "glew/glew.h"
#include "glfw/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <iostream>

using namespace std;

//注：a, b两个向量确定一个面，叉乘求出垂直于面的向量c，然后a, c两个向量确定一个面，叉乘求出垂直于面的向量d。
//a, c, d互相垂直，生成一个坐标系。在三维空间上由上向量确定的坐标系将产生一个相对于世界坐标在指向y轴方向的摄像机坐标系。
//偏航角和俯仰角设置和这个坐标系无关。只是会由偏航角和俯仰角确定新的摄像机位置，
//确保我们的方向向量一直为固定值，单位长度，但是其方向改变了（3D中所有向量都是三维的，有长度也有角度）。
//然后由新的方向向量确定摄像机坐标系。
//————————————————
//版权声明：本文为CSDN博主「xhm01291212」的原创文章，遵循 CC 4.0 BY - SA 版权协议，转载请附上原文出处链接及本声明。
//原文链接：https ://blog.csdn.net/xhm01291212/article/details/79175887

// 偏转初始化为 - 90.0度，因为偏转为0.0会导致方向向量指向右侧，所以我们最初会向左旋转一点。
//欧拉角：俯仰角（pitch）、偏航角（yaw）和滚转角（roll）。
//GLfloat pitch = 0.0f;            //俯仰角：绕X轴旋转，机头上下俯仰,
//								 //俯仰角（pitch）：摄像机坐标系x轴与水平面（世界坐标系）x轴夹角，当摄像机x轴位于水平面x轴之上（y轴正半轴）时，
//								 //俯仰角上升（抬头），否则低头。
//GLfloat yaw = -90.0f;            //偏航角：绕Y轴旋转，机头左右偏转
//								 //偏航角（yaw）：摄像机坐标系y轴与水平面y轴夹角，当指向x正半轴时为正，指向x负半轴时为负。
//GLfloat roll = 0.0f;             //翻滚角：绕Z轴旋转，机头左右翻滚
//								 //滚转角（roll）：摄像机坐标系z轴与水平面z轴夹角，当右翼上升为正，左翼上升为负。

//定义相机移动的几个可能选项。用作抽象以远离特定于窗口系统的输入方法
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const GLfloat PITCH = 0.0f;
const GLfloat YAW = -90.0f;
const GLfloat ROLL = 0.0f;
const GLfloat SPEED = 36.0f;
const GLfloat ZOOM = 45.0f;
const GLfloat SENSITIVITY = 0.25f;//缩放的快慢程度

//一个抽象的camera类，用于处理输入并计算OpenGL中使用的相应Euler角（欧拉角）、向量和矩阵
class Camera
{
public:
	//相机属性
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	//欧拉角
	GLfloat Pitch;//俯仰
	GLfloat Yaw;  //偏航
	GLfloat Roll; //翻滚
	//相机选项
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;//鼠标偏移量系数
	GLfloat Zoom;
	
	//使用向量构造
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat pitch = PITCH, GLfloat yaw = YAW) : Front(glm::vec3(0.0f, 0.0f, -2.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		this->Position = position;
		this->WorldUp = up;
		this->Pitch = pitch;
		this->Yaw = yaw;
		this->UpdateCameraVectors();
	}
	//使用标量构造
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat pitch, GLfloat yaw) : Front(glm::vec3(0.0f, 0.0f, -2.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		this->Position = glm::vec3(posX, posY, posZ);
		this->WorldUp = glm::vec3(upX, upY, upZ);
		this->Pitch = pitch;
		this->Yaw = yaw;
		this->UpdateCameraVectors();
	}
	//返回使用Eular角和LookAt矩阵计算的视图矩阵
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
	}
	//处理从任何类似键盘的输入系统接收的输入。接受摄像机定义枚举形式的输入参数（从窗口系统中提取）
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->MovementSpeed * deltaTime * 0.1;
		if (direction == FORWARD)
		{
			this->Position += this->Front * velocity;
		}
		if (direction == BACKWARD)
		{
			this->Position -= this->Front * velocity;
		}
		if (direction == LEFT)
		{
			this->Position -= this->Right * velocity;
			//this->Position -= glm::cross(this->Front, this->Up) * velocity;
		}
		if (direction == RIGHT)
		{
			this->Position += this->Right * velocity;
			//this->Position += glm::cross(this->Front, this->Up) * velocity;
		}
	}
	//处理从鼠标输入系统接收的输入。在x和y方向都需要偏移值。
	void ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true)
	{
		xOffset *= this->MouseSensitivity;
		yOffset *= this->MouseSensitivity;

		this->Yaw += xOffset;  //偏航，左右是X轴
		this->Pitch += yOffset;//俯仰，上下是Y轴

		//确保当俯仰角超出+-90度，屏幕不会翻转
		if (constrainPitch)
		{
			if (this->Pitch > 89.0f)
			{
				this->Pitch = 89.0f;
			}
			if (this->Pitch < -89.0f)
			{
				this->Pitch = -89.0f;
			}
		}

		//使用更新的Eular角度更新前、右和上向量
		this->UpdateCameraVectors();
	}

	//处理从鼠标滚轮事件接收的输入。只需要输入垂直车轮轴
	void ProcessMouseScroll(GLfloat zOffset)
	{
		if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
		{
			this->Zoom -= zOffset;
		}
		if (this->Zoom <= 1.0f)
		{
			this->Zoom = 1.0f;
		}
		if (this->Zoom >=45.0f)
		{
			this->Zoom = 45.0f;
		}
	}
protected:
private:
	//根据相机（更新的）Eular角度计算前向量
	void UpdateCameraVectors()
	{
		//计算新的前向量
		glm::vec3 front;
		front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		front.y = sin(glm::radians(this->Pitch));
		front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		this->Front = glm::normalize(front);//attention
		//同时重新计算右上向量
		//规范化向量，因为向量的长度越接近0，向上或向下查找的次数越多，移动速度就越慢。
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
		this->Up = glm::normalize(glm::cross(this->Right, this->Front));  //cross如果参数交换，方向相反
	}
};


#endif // !_CAMERA_H_



//enum Camera_Movement {
//	FORWARD,
//	BACKWARD,
//	LEFT,
//	RIGHT
//};
//
//// Default camera values
//const GLfloat YAW = -90.0f;
//const GLfloat PITCH = 0.0f;
//const GLfloat SPEED = 3.0f;
//const GLfloat SENSITIVTY = 0.25f;
//const GLfloat ZOOM = 45.0f;
//
//
//// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
//class Camera
//{
//public:
//	// Camera Attributes
//	glm::vec3 Position;
//	glm::vec3 Front;
//	glm::vec3 Up;
//	glm::vec3 Right;
//	glm::vec3 WorldUp;
//	// Eular Angles
//	GLfloat Yaw;
//	GLfloat Pitch;
//	// Camera options
//	GLfloat MovementSpeed;
//	GLfloat MouseSensitivity;
//	GLfloat Zoom;
//
//	// Constructor with vectors
//	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
//	{
//		this->Position = position;
//		this->WorldUp = up;
//		this->Yaw = yaw;
//		this->Pitch = pitch;
//		this->updateCameraVectors();
//	}
//	// Constructor with scalar values
//	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
//	{
//		this->Position = glm::vec3(posX, posY, posZ);
//		this->WorldUp = glm::vec3(upX, upY, upZ);
//		this->Yaw = yaw;
//		this->Pitch = pitch;
//		this->updateCameraVectors();
//	}
//
//	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
//	glm::mat4 GetViewMatrix()
//	{
//		return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
//	}
//
//	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
//	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
//	{
//		GLfloat velocity = this->MovementSpeed * deltaTime;
//		if (direction == FORWARD)
//			this->Position += this->Front * velocity;
//		if (direction == BACKWARD)
//			this->Position -= this->Front * velocity;
//		if (direction == LEFT)
//			this->Position -= this->Right * velocity;
//		if (direction == RIGHT)
//			this->Position += this->Right * velocity;
//	}
//
//	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
//	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
//	{
//		xoffset *= this->MouseSensitivity;
//		yoffset *= this->MouseSensitivity;
//
//		this->Yaw += xoffset;
//		this->Pitch += yoffset;
//
//		// Make sure that when pitch is out of bounds, screen doesn't get flipped
//		if (constrainPitch)
//		{
//			if (this->Pitch > 89.0f)
//				this->Pitch = 89.0f;
//			if (this->Pitch < -89.0f)
//				this->Pitch = -89.0f;
//		}
//
//		// Update Front, Right and Up Vectors using the updated Eular angles
//		this->updateCameraVectors();
//	}
//
//	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
//	void ProcessMouseScroll(GLfloat yoffset)
//	{
//		if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
//			this->Zoom -= yoffset;
//		if (this->Zoom <= 1.0f)
//			this->Zoom = 1.0f;
//		if (this->Zoom >= 45.0f)
//			this->Zoom = 45.0f;
//	}
//
//private:
//	// Calculates the front vector from the Camera's (updated) Eular Angles
//	void updateCameraVectors()
//	{
//		// Calculate the new Front vector
//		glm::vec3 front;
//		front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
//		front.y = sin(glm::radians(this->Pitch));
//		front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
//		this->Front = glm::normalize(front);
//		// Also re-calculate the Right and Up vector
//		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
//		this->Up = glm::normalize(glm::cross(this->Right, this->Front));
//	}
//};
//
//#endif // !_CAMERA_H_
////
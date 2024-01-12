#pragma once
#ifndef  CAMERA_H
#define  CAMERA_H
#include"utils.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<string>
#include <vector>
enum class Camera_Movement : std::uint32_t {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 10.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
class Camera
{
public:
	// camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// euler Angles
	float Yaw;
	float Pitch;
	// camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;
	inline static std::vector<std::string> mode_name{
		u8"环绕"u8,
		u8"自由"u8,
		u8"第三人称"u8,
	};
	int mode=2;//0 环绕 1 自由 2 第三人称
	//第三人称属性
	glm::vec3 Target=glm::vec3(0,0.2,0);
	float distance=10;
	Camera(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 up = glm::vec3(0, 1, 0), float yaw = YAW, float pitch = PITCH) :
		Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM),
		Position(position), WorldUp(up), Yaw(yaw), Pitch(pitch) {
		updateCameraVectors();
	}
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
		Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM),
		Position(glm::vec3(posX, posY, posZ)), WorldUp(glm::vec3(upX, upY, upZ)), Yaw(yaw), Pitch(pitch) {
		updateCameraVectors();
	}
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		using enum Camera_Movement;
		float velocity = MovementSpeed * deltaTime;
		if (mode == 0) {
			if (direction == FORWARD)
				Position += Up * velocity;
			if (direction == BACKWARD)
				Position -= Up * velocity;
			if (direction == LEFT)
				Position -= Right * velocity;
			if (direction == RIGHT)
				Position += Right * velocity;
			updateCameraVectors();
		}
		if (mode == 1) {

			if (direction == FORWARD)
				Position += Front * velocity;
			if (direction == BACKWARD)
				Position -= Front * velocity;
			if (direction == LEFT)
				Position -= Right * velocity;
			if (direction == RIGHT)
				Position += Right * velocity;
		}
		if (mode == 2) {
			glm::vec3 horizontalFront = glm::normalize(glm::vec3(Front.x, 0.0f, Front.z)); // XoZ 平面的前方向
			if (direction == FORWARD)
				Target += horizontalFront * velocity;
			if (direction == BACKWARD)
				Target -= horizontalFront * velocity;
			if (direction == LEFT)
				Target -= Right * velocity;
			if (direction == RIGHT)
				Target += Right * velocity;
			Position = Target - Front * distance;
		}
	}

	// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		if (mode == 1) {

			xoffset *= MouseSensitivity;
			yoffset *= MouseSensitivity;

			Yaw += xoffset;
			Pitch += yoffset;

			// make sure that when pitch is out of bounds, screen doesn't get flipped
			if (constrainPitch)
			{
				if (Pitch > 89.0f)
					Pitch = 89.0f;
				if (Pitch < -89.0f)
					Pitch = -89.0f;
			}
			updateCameraVectors();
		}
		if (mode == 2) {

			xoffset *= MouseSensitivity;
			yoffset *= MouseSensitivity;

			Yaw += xoffset;
			Pitch += yoffset;

			// make sure that when pitch is out of bounds, screen doesn't get flipped
			if (constrainPitch)
			{
				if (Pitch > 0.f)
					Pitch = 0.f;
				if (Pitch < -89.0f)
					Pitch = -89.0f;
			}
			// update Front, Right and Up Vectors using the updated Euler angles
			updateCameraVectors();
		}
	}

	// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		if (mode == 0 || mode == 1) {
			Zoom -= (float)yoffset;
			if (Zoom < 1.0f)
				Zoom = 1.0f;
			if (Zoom > 45.0f)
				Zoom = 45.0f;

		}
		if (mode == 2) {
			distance -= (float)yoffset;
			distance = std::max(distance, 1.f);
			distance = std::min(distance, 20.f);
			updateCameraVectors();
		}
	}
	void setMode(int mode) {
		this->mode = mode;
		if (mode==1) {//从环绕切回自由 根据当前视角更新YAW PITCH
			glm::vec3 direction;
			direction.x = Front.x; // 由于环绕模式中Front指向原点，因此需要取反
			direction.y = 0; // Yaw只考虑水平分量
			direction.z = Front.z; // 同上，取反

			Yaw = glm::degrees(atan2(direction.z, direction.x));

			float distance = glm::length(glm::vec2(direction.x, direction.z));
			Pitch = glm::degrees(atan2(Front.y, distance));
		}
		updateCameraVectors();
	}
private:
	// calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		if (mode == 0) {
			glm::vec3 front;
			front = glm::vec3(0)-Position;
			Front = glm::normalize(front);
			// also re-calculate the Right and Up vector
			Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			Up = glm::normalize(glm::cross(Right, Front));
		}
		if (mode == 1) {
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
		}
		if (mode == 2) {
			glm::vec3 front;
			front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			front.y = sin(glm::radians(Pitch));
			front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			Front = glm::normalize(front);
			// also re-calculate the Right and Up vector
			Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			Up = glm::normalize(glm::cross(Right, Front));

			Position = Target - Front * distance;
		}
	}
};

#endif // ifndef CAMERA_H

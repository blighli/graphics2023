#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 rotateAbout(glm::vec3 axis, float radians)
{
	float c = cos(radians);
	float s = sin(radians);
	float t = 1.0 - c;
	glm::vec3 a = normalize(axis);

	return glm::mat4(
		glm::vec4(t * a.x * a.x + c, t * a.x * a.y - s * a.z, t * a.x * a.z + s * a.y, 0.0),
		glm::vec4(t * a.x * a.y + s * a.z, t * a.y * a.y + c, t * a.y * a.z - s * a.x, 0.0),
		glm::vec4(t * a.x * a.z - s * a.y, t * a.y * a.z + s * a.x, t * a.z * a.z + c, 0.0),
		glm::vec4(0.0, 0.0, 0.0, 1.0)
	);
}

class Camera {
public:
	glm::vec3 dir;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 pos;
	glm::vec3 polarPos;
	glm::vec3 lookingAt;
	float zoomLimit;
	float PI = atan(1.0f) * 4.0f;

	Camera() : dir(glm::vec3(0, 0, -1)),
		right(glm::vec3(1, 0, 0)),
		up(glm::vec3(0, 1, 0)),
		polarPos(glm::vec3(0, 0, 0)),
		lookingAt(glm::vec3(0, 0, 0))
	{
		pol2cart();
	}
	Camera(glm::vec3 _polarPos, glm::vec3 _lookingAt, float _zoomLimit) : polarPos(_polarPos),
		lookingAt(_lookingAt),
		zoomLimit(_zoomLimit)
	{
		pol2cart();
	}

	void pol2cart() {
		polarPos.z += 0.0001;
		if (polarPos.z < zoomLimit)
			polarPos.z = zoomLimit + 0.3;
		polarPos.z = glm::clamp(polarPos.z, zoomLimit, 900.0f);
		polarPos.x = glm::clamp(polarPos.x, 0.1f * PI, 0.9f * PI);
		polarPos.y = glm::clamp(polarPos.y, -0.9f * PI, 0.9f * PI);

		pos = polarPos.z * glm::vec3(sin(polarPos.x) * cos(polarPos.y), sin(polarPos.y) * sin(polarPos.x), cos(polarPos.x)) + lookingAt;
		dir = glm::normalize(pos - lookingAt);
		right = glm::normalize(glm::cross(dir, glm::vec3(0, 0, 1)));
		up = glm::normalize(glm::cross(right, dir));
	}
	glm::mat4 getMatrix()
	{
		glm::mat4 cameraRotation = glm::mat4(
			glm::vec4(right, 0),
			glm::vec4(up, 0),
			glm::vec4(-dir, 0),
			glm::vec4(0, 0, 0, 1));

		glm::mat4 translation = glm::mat4(
			glm::vec4(1, 0, 0, 0),
			glm::vec4(0, 1, 0, 0),
			glm::vec4(0, 0, 1, 0),
			glm::vec4(pos, 1));

		return glm::transpose(cameraRotation) * translation;
	}

	void translateCamera(float up, float around, float theOtherOne) {
		polarPos += glm::vec3(up, around, theOtherOne);
		pol2cart();
		printf("polarPos: %f %f %f\n", polarPos.x, polarPos.y, polarPos.z);
		printf("pos: %f %f %f\n", pos.x, pos.y, pos.z);
	}

	void moveCamera(float x, float y, float z) {
		lookingAt += x * right + y * up + z * dir;
		pol2cart();
		printf("lookingAt: %f %f %f\n", lookingAt.x, lookingAt.y, lookingAt.z);
		printf("pos: %f %f %f\n", pos.x, pos.y, pos.z);
	}
};
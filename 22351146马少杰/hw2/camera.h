#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace glm;
mat4 rotateAbout(vec3 axis, float radians)
{
	float c = cos(radians);
	float s = sin(radians);
	float t = 1.0 - c;
	vec3 a = normalize(axis);

	return mat4(
		vec4(t * a.x * a.x + c, t * a.x * a.y - s * a.z, t * a.x * a.z + s * a.y, 0.0),
		vec4(t * a.x * a.y + s * a.z, t * a.y * a.y + c, t * a.y * a.z - s * a.x, 0.0),
		vec4(t * a.x * a.z - s * a.y, t * a.y * a.z + s * a.x, t * a.z * a.z + c, 0.0),
		vec4(0.0, 0.0, 0.0, 1.0)
	);
}

class Camera {
public:
	vec3 dir;
	vec3 up;
	vec3 right;
	vec3 pos;
	vec3 polarPos;
	vec3 lookingAt;
	float zoomLimit;
	float PI = atan(1.0f) * 4.0f;

	Camera() : dir(vec3(0, 0, -1)),
		right(vec3(1, 0, 0)),
		up(vec3(0, 1, 0)),
		polarPos(vec3(0, 0, 0)),
		lookingAt(vec3(0, 0, 0))
	{
		pol2cart();
	}
	Camera(vec3 _polarPos, vec3 _lookingAt, float _zoomLimit) : polarPos(_polarPos),
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
		polarPos.x = glm::clamp(polarPos.x, -0.85f * PI, 0.85f * PI);
		// polarPos.y = glm::clamp(polarPos.y, -0.475f * PI, 0.475f * PI);

		pos = polarPos.z * vec3(sin(polarPos.x) * cos(polarPos.y), sin(polarPos.y) * sin(polarPos.x), cos(polarPos.x)) + lookingAt;
		dir = normalize(pos - lookingAt);
		right = normalize(cross(dir, vec3(0, 0, 1)));
		up = normalize(cross(right, dir));
	}
	mat4 getMatrix()
	{
        mat4 cameraRotation = mat4(
            vec4(right, 0),
            vec4(up, 0),
            vec4(-dir, 0),
            vec4(0, 0, 0, 1));

        mat4 translation = mat4(
            vec4(1, 0, 0, 0),
            vec4(0, 1, 0, 0),
            vec4(0, 0, 1, 0),
            vec4(pos, 1));

        return transpose(cameraRotation) * translation;
	}

	void translateCamera(float up, float around, float theOtherOne) {
		polarPos += vec3(up, around, theOtherOne);
		pol2cart();
		// printf("polarPos: %f %f %f\n", polarPos.x, polarPos.y, polarPos.z);
		// printf("pos: %f %f %f\n", pos.x, pos.y, pos.z);
	}

	void moveCamera(float x, float y, float z) {
		lookingAt += x * right + y * up + z * dir;
		pol2cart();
		// printf("lookingAt: %f %f %f\n", lookingAt.x, lookingAt.y, lookingAt.z);
		// printf("pos: %f %f %f\n", pos.x, pos.y, pos.z);
	}
};
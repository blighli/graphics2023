#include "sphere.h"

Sphere::Sphere()
{
}

Sphere::Sphere(int x_s, int y_s, glm::vec3 c, float r) :Y_SEGMENTS(y_s), X_SEGMENTS(x_s),
core(c), radius(r)
{
}


Sphere::Sphere(int x_s, int y_s)
{
	X_SEGMENTS = x_s;
	Y_SEGMENTS = y_s;
}

Sphere::~Sphere()
{
}

std::vector<float> Sphere::getVertices()
{
	const GLfloat PI = 3.14159f;
	std::vector<float> sphereVertices;
	for (int y = 0; y <= Y_SEGMENTS; y++)
	{
		for (int x = 0; x <= X_SEGMENTS; x++)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = radius * std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI) + core.x;
			float yPos = radius * std::cos(ySegment * PI) + core.y;
			float zPos = radius * std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI) + core.z;
			sphereVertices.push_back(xPos);
			sphereVertices.push_back(yPos);
			sphereVertices.push_back(zPos);
		}
	}
	return sphereVertices;
}

std::vector<int> Sphere::getIndices()
{
	std::vector<int> sphereIndices;
	for (int j = 0; j < Y_SEGMENTS; j++)
	{
		for (int i = 0; i < X_SEGMENTS; i++)
		{
			sphereIndices.push_back((j + 1) * (X_SEGMENTS + 1) + i);
			sphereIndices.push_back((j + 1) * (X_SEGMENTS + 1) + i + 1);
			sphereIndices.push_back(j * (X_SEGMENTS + 1) + i + 1);

			sphereIndices.push_back(j * (X_SEGMENTS + 1) + i + 1);
			sphereIndices.push_back(j * (X_SEGMENTS + 1) + i);
			sphereIndices.push_back((j + 1) * (X_SEGMENTS + 1) + i);
		}
	}
	return sphereIndices;
}

std::vector<float> Sphere::getNormals()
{
	const GLfloat PI = 3.14159f;
	std::vector<float> sphereNormals;
	for (int y = 0; y <= Y_SEGMENTS; y++)
	{
		for (int x = 0; x <= X_SEGMENTS; x++)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			glm::vec3 temp = glm::normalize(glm::vec3(xPos, yPos, zPos));
			sphereNormals.push_back(temp.x);
			sphereNormals.push_back(temp.y);
			sphereNormals.push_back(temp.z);
		}
	}
	return sphereNormals;
}

std::vector<float> Sphere::getTexCoords()
{
	const GLfloat PI = 3.14159f;
	std::vector<float> sphereTexCoords;
	for (int y = 0; y <= Y_SEGMENTS; y++)
	{
		for (int x = 0; x <= X_SEGMENTS; x++)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			sphereTexCoords.push_back(1 - xSegment);
			sphereTexCoords.push_back(ySegment);
		}
	}
	return sphereTexCoords;
}

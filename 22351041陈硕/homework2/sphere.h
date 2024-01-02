#pragma once
#include <vector>
#include <cmath>
#include <glad\glad.h>
#include "glm/glm.hpp"
#ifndef SPHERE_H
#define SPHERE_H



class Sphere
{
public:
	int Y_SEGMENTS = 20;
	int X_SEGMENTS = 20;
	glm::vec3 core = glm::vec3(0.0, 0.0, 0.0);
	float radius = 1.0f;
	Sphere();
	Sphere(int x_s, int y_s, glm::vec3 c, float r);
	Sphere(int x_s, int y_s);
	~Sphere();


	std::vector<float> getVertices();
	std::vector<int> getIndices();
	std::vector<float> getNormals();
	std::vector<float> getTexCoords();
};
#endif
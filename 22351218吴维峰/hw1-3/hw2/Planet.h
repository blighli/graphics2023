#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Config.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <math.h>
#include <vector>

class Planet
{
public:
	Planet();

	GLfloat R = 1;

	void init(char* path, float R);
	void draw(glm::mat4 projection, glm::mat4 view);
	void setTRevolutionTSpin(float TRevolution, float TSpin);
	void setModel(float T,
		float rRevolution,
		float xStar, float yStar, float zStar,
		float xRotation, float yRotation, float zRotation,
		float xSpin, float ySpin, float zSpin);
	void setStarLighting(glm::vec3 viewPos, float ambient1, float ambient2, float ambient3,
		float diffuse1, float diffuse2, float diffuse3,
		float specular1, float specular2, float specular3);
	void setStarJudge(bool starJudge);

	glm::mat4 model;
protected:
	const int Y_SEGMENTS = 25;
	const int X_SEGMENTS = 25;
	std::vector<float> sphereVertices;
	std::vector<int> sphereIndices;

	float TRevolution, TSpin;
	float xStar, yStar, zStar;

	unsigned int diffuseMap, specularMap;
	Shader* planetShader;
	unsigned int VBO, VAO;
	GLuint element_buffer_object;
	bool starJudge;

	void initVertex();
	unsigned int loadTexture(char* path);
};


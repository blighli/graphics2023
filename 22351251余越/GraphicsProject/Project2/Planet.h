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

	GLfloat R = 1;//星球半径

	void init(char* path, float R);
	void draw(glm::mat4 projection, glm::mat4 view);
	void setTRevolutionTSpin(float TRevolution, float TSpin);
	void setModel(float T,//已过的时间
		float rRevolution,//公转半径
		float xStar, float yStar, float zStar,//公转所绕星坐标
		float xRotation, float yRotation, float zRotation,//公转轴
		float xSpin, float ySpin, float zSpin);//自转轴
	void setStarLighting(glm::vec3 viewPos, float ambient1, float ambient2, float ambient3,
		float diffuse1, float diffuse2, float diffuse3,
		float specular1, float specular2, float specular3);
	void setStarJudge(bool starJudge);

	glm::mat4 model;//位置矩阵
protected:
	const int Y_SEGMENTS = 25;
	const int X_SEGMENTS = 25;
	std::vector<float> sphereVertices;
	std::vector<int> sphereIndices;

	float TRevolution, TSpin;//公转周期，自转周期
	float xStar, yStar, zStar;//公转所绕恒星坐标

	unsigned int diffuseMap, specularMap;//纹理标号
	Shader* planetShader;//星球着色器
	unsigned int VBO, VAO;
	GLuint element_buffer_object;//EBO
	bool starJudge;

	void initVertex();
	unsigned int loadTexture(char* path);
};


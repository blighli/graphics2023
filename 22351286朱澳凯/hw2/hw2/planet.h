#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <shader.h>

#include <vector>
#include <iostream>

#define PI 3.1415926535
class Planet
{
public:

	Planet(GLfloat r);
	~Planet();
    glm::mat4 model;

    void Init(const char* textPath);
    void draw(glm::mat4 projection, glm::mat4 view, glm::vec3 viewPos, bool sun);
    GLfloat getR();
private:
	
    unsigned int loadTexture(const char* path);
    
    GLfloat R;
	const int Y_SEGMENTS = 25;
	const int X_SEGMENTS = 25;
	std::vector<float> sphereVertices;
	std::vector<int> sphereIndices;
	Shader* planetShader;//星球着色器
	unsigned int VBO, VAO;
	GLuint element_buffer_object;//EBO
    unsigned int diffuseMap, specularMap;
};

Planet::Planet(GLfloat r)
{
    this->R = r;
    planetShader = new Shader("./shaders/planet.vs", "./shaders/planet.fs");
}

Planet::~Planet()
{
}

void Planet::Init(const char* textPath)
{
    for (int y = 0; y <= Y_SEGMENTS; y++)
    {
        for (int x = 0; x <= X_SEGMENTS; x++)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = R * std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = R * std::cos(ySegment * PI);
            float zPos = R * std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float xTex = 1.0 * x / X_SEGMENTS;
            float yTex = 1 - 1.0 * y / Y_SEGMENTS;
            sphereVertices.push_back(xPos);
            sphereVertices.push_back(yPos);
            sphereVertices.push_back(zPos);
            sphereVertices.push_back(xPos);
            sphereVertices.push_back(yPos);
            sphereVertices.push_back(zPos);
            sphereVertices.push_back(xTex);
            sphereVertices.push_back(yTex);
        }
    }
    //生成球的Indices
    for (int i = 0; i < Y_SEGMENTS; i++)
    {
        for (int j = 0; j < X_SEGMENTS; j++)
        {
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);

            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //生成并绑定球体的VAO和VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //将顶点数据绑定至当前默认的缓冲中
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

    //EBO
    glGenBuffers(1, &element_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    diffuseMap = loadTexture(textPath);
    specularMap = loadTexture(textPath);

}

unsigned int Planet::loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID); // 设置为2D纹

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "path: " << path << std::endl;
        std::cout << "Failed to load texture" << std::endl;
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    // Set the texture wrapping parameters  设置纹理包装参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters  设置纹理过滤的参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return textureID;


}

void Planet::draw(glm::mat4 projection, glm::mat4 view, glm::vec3 viewPos,bool sun = false) {
    planetShader->setMat4("projection", projection);
    planetShader->setMat4("view", view);

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    glBindVertexArray(VAO);

    planetShader->use();
    planetShader->setVec3("lightPos", glm::vec3(0.0f, 0.0f, 0.0f));
    planetShader->setVec3("viewPos", viewPos);
    planetShader->setInt("material.diffuse", 0.5);
    planetShader->setInt("material.specular", 0.5);
    planetShader->setInt("material.shininess", 16.0f);
    planetShader->setMat4("model", model);
    planetShader->setBool("isSun", sun);
    
    glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
}
GLfloat Planet::getR() {
    return R;
}


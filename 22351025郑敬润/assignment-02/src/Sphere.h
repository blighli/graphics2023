#pragma once

#include <iostream>
#include <cmath>
#include <vector>

#include "config.h"

class Sphere
{
public:
	Sphere(){
        init(30); // 默认精度为 30
    }
	Sphere(const int precision){
        init(precision); // precision代表球体纵横划分的网格数，越大越球体精细
    }

	int getNumIndices() const { return numIndices; }
	void setNumIndices(const int prec) { numIndices = prec * prec * 6; }
	std::vector<unsigned int> getIndices() const { return indices; }
	std::vector<float> getVertices() const { return vertices; }

private:
	int numIndices;
	std::vector<unsigned int> indices;
	std::vector<float> vertices;
    float toRadians(float degrees) { return (degrees * 2.0f * PI) / 360.0f; }

private:
    void init(const int prec){
        setNumIndices(prec);
        vertices.resize((prec + 1) * (prec + 1) * 5);
        indices.resize(prec * prec * 6);
        int verticeIndex = 0;
        int indiceIndex = 0;

        //计算三角形顶点坐标
        for (int y = 0; y <= prec; y++){ // row
            for (int x = 0; x <= prec; x++){ // column
                float xSegment = (float)x / (float)prec;
                float ySegment = (float)y / (float)prec;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                //顶点坐标
                vertices[verticeIndex++] = (xPos);
                vertices[verticeIndex++] = (yPos);
                vertices[verticeIndex++] = (zPos);
                
                // 贴图坐标
                vertices[verticeIndex++] = ((float)x / (float)prec);
                vertices[verticeIndex++] = ((float)y / (float)prec);

            }
        }

        for (int i = 0; i < prec; i++){
            for (int j = 0; j < prec; j++){
                indices[indiceIndex++] = (i * (prec + 1) + j);
                indices[indiceIndex++] = ((i + 1) * (prec + 1) + j);
                indices[indiceIndex++] = ((i + 1) * (prec + 1) + j + 1);
                                        
                indices[indiceIndex++] = (i * (prec + 1) + j);
                indices[indiceIndex++] = ((i + 1) * (prec + 1) + j + 1);
                indices[indiceIndex++] = (i * (prec + 1) + j + 1);
            }
        }

        // std::vector<float> sphereNormals(sphereVertices.size());
        // for (int i = 0; i < sphereVertices.size(); i+=9){
        //     glm::vec3 e1(sphereVertices[i+3]-sphereVertices[i], sphereVertices[i+4]-sphereVertices[i+1], sphereVertices[i+5]-sphereVertices[i+2]); // v2-v1
        //     glm::vec3 e2(sphereVertices[i+6]-sphereVertices[i], sphereVertices[i+7]-sphereVertices[i+1], sphereVertices[i+8]-sphereVertices[i+2]); // v3-v1
        //     glm::vec3 n = glm::normalize(glm::cross(e1, e2)); // 法向量
        //     sphereNormals[i] = sphereNormals[i+3] = sphereNormals[i+6] = n.x;
        //     sphereNormals[i+1] = sphereNormals[i+4] = sphereNormals[i+7] = n.y;
        //     sphereNormals[i+2] = sphereNormals[i+5] = sphereNormals[i+8] = n.z;
        // }
    }
};


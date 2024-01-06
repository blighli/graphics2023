#include <iostream>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define PI 3.1415926535897

using std::vector;

class Sphere{
public:
    vector<float> vertices;
    vector<float> normals;
    vector<int> indices;
    vector<float> texcoords;
    Sphere(float sectorCount = 64.0f, float stackCount = 32.0f, float radius = 1.0f){
        float sectorStep = 2 * PI / sectorCount;
        float stackStep = PI / stackCount;
        float widthStep = 1.0f / sectorCount;
        float heightStep = 1.0f / stackCount;
        float radiusInv = 1 / radius;

        for (int i = 0; i <= stackCount; ++i){
            float stackAngle = i * stackStep;
            float y = radius * cos(stackAngle);
            float v = i * heightStep;
            for (int j = 0; j <= sectorCount; ++j){
                float sectorAngle = j * sectorStep;
                float z = radius * sin(stackAngle) * cos(sectorAngle);
                float x = radius * sin(stackAngle) * sin(sectorAngle);
                //vertices
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
                //normals，球上一点的法向量，过球心（0，0，0），因此等同于xy
                normals.push_back(x);
                normals.push_back(y);
                normals.push_back(z);
                //u、v映射
                float u = j * widthStep;
                texcoords.push_back(u);
                texcoords.push_back(v);
            }
        }
        //索引
        for (int i = 0; i < stackCount; ++i){
            for (int j  = 0; j < sectorCount; ++j){
                indices.push_back((int)(i*(sectorCount+1)+j));
                indices.push_back((int)((i+1)*(sectorCount+1)+j+1));
                indices.push_back((int)(i*(sectorCount+1)+j+1));
                indices.push_back((int)(i*(sectorCount+1)+j));
                indices.push_back((int)((i+1)*(sectorCount+1)+j));
                indices.push_back((int)((i+1)*(sectorCount+1)+j+1));
            }
        }
    }
};